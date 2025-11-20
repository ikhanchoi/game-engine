
I used ChatGPT with copying and pasting code blocks fewer than 10 lines for convenience.

# 1. Core

## 1.1. Execution

이 게임엔진에서 여러가지 일을 수행하는 주체는 매니저와 시스템이다.
각 매니저와 시스템은 하나의 이벤트 디스패처와 하나의 커맨드 버퍼를 가지고 일을 처리하게 할 것이다.
한 프레임에서 일어나는 대략적인 이벤트 및 커맨드 관련 일들의 플로우는 다음과 같으며, 매니저나 시스템도 다음 네 층위의 메소드들을 갖게 된다.

1. Detect events
2. Handle events (by dispatch)
3. Push commands
4. Execute commands (by flush)

__예시: 엔티티 생성/삭제__

에디터의 UI 시스템에서 엔티티 생성/삭제 버튼이 눌린 상황을 가정하자.
구체적으로는, UI 시스템이 UI 요소들을 순회하며 버튼이 눌렸다는 이벤트를 UI 시스템의 이벤트 디스패처에 모은 것이다(Detect events).

```c++
// in UI System
if (ImGui::Button("Remove Entity")) {
	publish<onRemoveEntityButtonPressed>(entity);
}
```

UI 시스템의 순회 이후 UI 시스템의 `dispatch`가 호출되면 UI 시스템이 모아놓은 이벤트들을 구독하던 이벤트 핸들러들이 호출된다.
핸들러 메소드의 이름은 주로 전치사 on으로 시작한다.
여기서 구독되어 있어야 할 이벤트 핸들러는 엔티티 매니저가 갖고 있는 엔티티 생성/삭제 핸들러이다(Handle events).

```c++
void EntityManager::onRemoveEntityButtonPressed(const removeEntityButtonPressedEvent& event) {
	removeEntity(event.entity);
}
```

엔티티 생성의 경우 즉시 일을 수행할 수도 있지만, 엔티티 삭제의 경우 핸들러가 즉시 삭제를 실행하는 메소드를 호출해버리면 같은 프레임의 다른 곳에서 삭제된 엔티티를 참조할 때 큰일이 나므로, 커맨드를 통해 업데이트 루프의 마지막 스테이지에서 실제 삭제가 이루어져야 한다.
따라서 엔티티 삭제 핸들러는 다음과 같이 커맨드를 만들어 엔티티 매니저 자신의 커맨드 버퍼에 커맨드를 푸시한다(Push commands).


```c++
void EntityManager::removeEntity(Handle<Entity> entity, bool immediate = false) {
	if (immediate)
		// remove entity from scene graph and destroy entity in memory
	else
		push<Command>(
			[this, entity] { removeEntity(entity, true); }
		);
}
```
마지막으로 업데이트 루프에서 엔티티 매니저의 `flush`가 호출되면 커맨드에 등록되었던 메소드가 실행된다(Execute commands).


### Events

이벤트 디스패처는 다음과 같이 생겼다.


```c++
class EventDispatcher {
	std::vector<std::unique_ptr<EventBase>> events;
	std::unordered_map<std::type_index, std::vector<std::function<void(const EventBase&)>>> listeners;
public:
	template <typename EventType>
	void subscribe(const std::function<void(const EventType&)>& handler);
	template <typename EventType, typename... Args>
	void publish(Args&&... args);
	void dispatch();
}
```

### Commands

Undo-redo 시스템에 관한 건 본래 에디터의 구체적인 구현 부분에 들어가야 해서 코어에 들어가면 안되지만, `UndoableCommand`만은 예외이다.
온갖 매니저와 시스템에서 Redo 함수를 등록할 수 있어야 하기 때문이다.
이 클래스는 당연하지만 Undo-redo 시스템을 참조하지 않게 해서 코어가 에디터에 의존하지 않게 해야 한다.

```c++
class CommandBase {
protected:
	std::function<void()> perform;
	explicit CommandBase(std::function<void()> function) : perform(std::move(function)) {}
public:
	void execute() const { perform(); }
	virtual ~CommandBase() = 0;
};

class Command final : public CommandBase {
public:
	explicit Command(std::function<void()> function) : CommandBase(std::move(function)) {}
};

class UndoableCommand final : public CommandBase {
	std::function<void()> revert;
public:
	explicit UndoableCommand(std::function<void()> perform, std::function<void()> revert)
		: CommandBase(std::move(perform)), revert(std::move(revert)) {}
	void undo() const { revert(); }
	std::unique_ptr<UndoableCommand> clone() const {
		return std::make_unique<UndoableCommand>(perform, revert);
	}
};
```

커맨드버퍼는 `executing`을 넣어 놓아 커맨드의 내용물에서 새로운 커맨드가 만들어지는 꼬리물기나 다음 프레임으로 넘어가는 불상사를 막는다.


```c++
class CommandBuffer {
	std::vector<std::unique_ptr<CommandBase>> commands; // this may be gonna be a typed map like event dispatcher.
	bool executing = false;
public:
	// ....
```

## 1.2. Memory

### Handles

포인터의 역할을 대신하는 템플릿 기반 핸들.
핸들에 의해 가리켜지며 스토리지에 저장될 수 있는 객체를 이 엔진에서는 오브젝트라고 부르고 `Object`라는 추상클래스를 상속하는 것으로 결정할 것이다.
스토리지에 접근하거나 시스템 및 매니저가 정보를 주고받을 때 항상 사용될 것이다.
invalid 핸들은 지원하지 않게 하였다.
`uint32_t` 두 개로 정확히 8바이트라 메모리적으로도 매우 좋다.

```c++
template <typename ObjectType>
struct Handle {
    uint32_t id;
    uint32_t generation;
	explicit Handle(uint32_t id, uint32_t generation) : id(id), generation(generation) {}
	bool operator == (const Handle& other) const { return id == other.id && generation == other.generation; }
	bool operator != (const Handle& other) const { return !(*this == other); }
};
```

핸들을 `std::unordered_map`의 키로 쓰려면 해시가 있어야 해서 해시함수의 구현도 있다.


```c++
namespace std {
	template <typename ObjectType>
	struct hash<Handle<ObjectType>> { // ...
```

### Storages

스토리지는 직관적으로 메모리다.
스토리지에서 핸들로 관리될 수 있다는 것은 곧 현재 메모리에 올라온 오브젝트라는 뜻이다.
스토리지는 할당자의 타입이 실제로 무엇인지, 즉 어떤 할당 알고리즘을 사용하는지 시스템과 매니저가 몰라도 되도록 할당자를 감싸는 역할을 한다.
할당자의 타입은 스토리지를 생성할 때만 알면 된다.


```c++
template <typename ObjectType>
class Storage {
	std::unique_ptr<Allocator<ObjectType>> allocator;
	explicit Storage(std::unique_ptr<Allocator<ObjectType>> allocator) : allocator(std::move(allocator)) {}
public:
	template <template <typename> class AllocatorType>
	static Storage make(size_t initialCapacity = 64) {
		return Storage(std::make_unique<AllocatorType<ObjectType>>(initialCapacity));
	}
	// ...
```


### Allocators


할당자의 추상클래스가 제공하는 기능들은 다음과 같다.


```c++
template <typename ObjectType>
class Allocator {
	friend class Storage<ObjectType>;

protected:
	virtual Handle<ObjectType> create() = 0;
	virtual std::unique_ptr<Allocator> clone() = 0;
	virtual void destroy(Handle<ObjectType>) = 0;
	virtual void clear() = 0;

	virtual bool valid(Handle<ObjectType>) = 0;
	virtual ObjectType* resolve(Handle<ObjectType>) = 0;
	virtual std::vector<Handle<ObjectType>> view() = 0;
	virtual void each(std::function<void(ObjectType*)>) = 0;

public:
	virtual ~Allocator() = 0;
};

```


아직 참조 카운트는 구현하지 않았다.




어차피 ObjectType 별로 할당자를 만들었으므로 each를 가상함수로 구현하더라도 핫루프에서 virtual call이 일어나지는 않는다.
패킹은 연속적 순회 배열을 만들기 위한 재배치이므로, “데이터 순서가 바뀌거나 필터링 대상이 변할 때”만 필요.


추후에 스크립트 시스템에서의 커스텀 타입 지원을 위해 동적으로 할당전략을 바꿀 수 있어야 하거나, 스크립트 내부에서 사용하기 위해 janitor로 구현된 scoped 핸들이 필요하게 될 가능성이 있다.








# 2. Runtime

실제 런타임에 돌아가는 ECS의 기본요소들과 렌더링, 물리, 애니메이션, 오디오, 인풋 등 게임의 기초적인 시스템들이 들어가 있는 곳이다.



## 2.1. Worlds

월드는 기본적으로 모든 스토리지를 소유하고, 매니저와 시스템들이 스토리지에 간섭할 수 있게 하는 장소로 정의하였다.
스토리지를 매니저가 소유하지 않게 설계하였다.
극장(Editor/Player)에 의해 소유되어, 활성화된 장면(Scene)에 지시된 대로 배우(Entity)들이 등장해 규칙에 맞추어 움직이는 무대.


```c++
class World {
	std::unordered_map<std::type_index, std::any> storage;
	std::unordered_map<std::type_index, std::unique_ptr<ManagerBase>> manager;
	std::unordered_map<std::type_index, std::unique_ptr<SystemBase>> system;
public:
	void update();
	// ...
```
이 코드의 `std::any`는 항상 `std::shared_ptr<Storage<...>>`이다.
얕은 복제를 염두에 두고 있기 때문에 공유 포인터를 사용한다.

추후 설명하겠지만 에디터는 에디트월드와 플레이월드, 기본적으로 총 두 개의 월드를 소유할 수 있다.
에디트월드는 에디터의 생성자와 소멸자에서 생성되고 소멸된다.
플레이월드는 플레이모드에 진입하고 퇴장했을 때 생성되고 소멸된다.
플레이모드로 진입하는 버튼이 에디트월드에서 눌리면, 플레이월드가 생성된 후 깊고 얕은 복사로 스토리지가 복제되고, 에디터에 필요 없고 런타임 게임에 필요한 시스템들과 매니저들(물리 등)만 초기화된다.

반면 아직 구현하지 않은 플레이어는 플레이월드 하나만을 소유하고, 나중에 멀티플레이게임을 구현하게 되면 월드가 훨씬 많아질 수 있다.

매니저와 시스템들끼리 서로 참조는 할 수 있는 상태로 코드를 짰지만, 가능하면 이벤트로만 소통하게끔 하자.


### Managers

객체들을 관리하는 신의 사자와 같은 존재들.
구체적으로, 매니저는 임포터와 할당자, 참조카운터 유틸을 이용함으로써 메모리 위 오브젝트의 생성/소멸 및 활성화/비활성화, 그리고 오브젝트들 간의 참조 및 소유관계 변경을 담당한다.
메모리적인 측면의 생성/소멸과 직관적 측면의 오브젝트의 활성화/비활성화를 구분하면 좋다.
플레이어가 다른 씬으로 이동해서 메모리에서 내려간 엔티티와 게임 로직에 의해 파괴되어 메모리에서 내려간 엔티티는 느낌이 다르다.
단순히 씬 전환으로 메모리에서 내려갔더라도 변경상태가 디스크에 박제된 몬스터가 있다면, 이를 비활성화라고 부를 수는 있어도 소멸로 부르면 슬프기 때문이다.
따라서 매니저가 메모리에서의 오브젝트 생성/소멸 메소드를 갖지만, 오브젝트가 메모리에 올라와 있을지 말지를 결정하는 것은 씬이므로, 객체지향적인 의미에서 오브젝트들의 수명(=출연 분량)을 책임지지는 않는다고 보았다.
오브젝트가 메모리에 올라와 있지 않아도 우리는 그 객체가 비활성화된 채 존재한다고 인식할 수 있으며, 관리자가 그 객체를 메모리(무대, 월드)에 올리고 내리는 역할만 할 뿐이다.
이런 의미에서 객체들은 기본적으로 어딘가에 소유되지 않는다.
이것이 스토리지를 매니저 밖에 존재시킨 이유.


```c++
class ManagerBase {
	World& world;
	std::unique_ptr<EventDispatcher> eventDispatcher;
	std::unique_ptr<CommandBuffer> commandBuffer;
	std::unordered_map<std::type_index, std::any> storage; // storage pointers that can be accessed by this manager instance.
	// ...
```
이 코드의 `std::any`는 항상 `std::weak_ptr<Handle<...>>`이다.
매니저가 변경가능한 권한을 가진 스토리지만을 캐시해왔다는 이미지.
씬 매니저는 추상클래스 `SceneBase`를 상속한 오브젝트들을, 컴포넌트 매니저는 추상클래스 `ComponentBase`를 상속한 오브젝트들을 관리하게끔 하였다.



### Systems

시스템은 매니저에 의해 활성화된 객체들에 접근하여 매 프레임 값을 갱신하거나 반환할 수 있다. (예: 물리시스템, 애니메이션 시스템, 이펙트 시스템)
객체에 접근해 값을 참조하는 건 다른 시스템도 할 수 있다.
오히려 참조만 하고 출력을 하는 시스템도 있다. (예: 렌더시스템, 오디오시스템)
렌더시스템은 렌더캐시, 스키닝매트릭스버퍼, 파티클버텍스버퍼, 스프라이트배치노드리스트, 유아이배치데이터 같은 렌더 파생 컴포넌트들을 수정한다.
이런 파생컴포넌트들은 사용자가 에디터를 통해 관리할 수 없으므로 인스펙터에 출력되지 않지만, 제대로 엔티티에 속해 있다.

1. Tick
2. Dispatch
3. Flush



## 2.2. Objects

스토리지에 저장될 수 있는 모든 것.
크게 네 종류로 분류된다.
씬이 엔티티를 갖고, 엔티티가 컴포넌트를 갖고, 컴포넌트가 리소스를 갖는 느낌으로, 상속관계는 예를 들어 다음과 같은 느낌이다.

```c++
class Scene final : public SceneBase;
class PersistentScene final : public SceneBase;

class Entity final : public EntityBase;
class PrefabEntity final : public EntityBase;

class ScriptComponent final : public ComponentBase;
class RenderComponent final : public ComponentBase;
class CameraComponent final : public ComponentBase;
class ColliderComponent final : public ComponentBase;
// ...

class PrimitiveResource final : public ResourceBase;
class ModelResource final : public ResourceBase;
class ShaderResource final : public ResourceBase;
// ...
```

### Scenes

현재 메모리에 올라와 있는, 그리고 올라와 있어야 하는 엔티티와 컴포넌트 간 소유 및 참조관계를 멤버 변수로 갖는 오브젝트.
씬이 활성화되어 메모리에 올라온 파라미터와 데이터는 시스템과의 상호작용으로 값이 변경될 수 있다.
씬이 비활성화되면 파라미터와 데이터는 메모리에서 해제되고 플레이 중 변경된 값은 잊힌다.
씬이 다시 활성화되면 컴포넌트와 리소스는 초깃값을 가지고 다시 메모리에 올라온다.
만약 파라미터나 데이터가 지속되길 원한다면 해당 컴포넌트나 리소스는 퍼시스턴트가 참조해야 한다.

씬이 소유하는 엔티티들 간의 계층관계를 가진 씬 그래프, 엔티티와 컴포넌트의 참조관계를 표현하는 마스크와 맵이 주요 멤버변수이다.
컴포넌트와 리소스 간 참조관계는 씬의 멤버가 아니라 컴포넌트의 멤버로서 관리된다.

```c++
class SceneBase {
	friend class EntityManager;
	friend class ComponentManager;

protected:
	Forest<Handle<Entity>> sceneGraph;
	std::unordered_map<Handle<Entity>, std::bitset<64>> componentMask; // dense
	std::unordered_map<Handle<Entity>, std::unordered_map<std::type_index, Handle<Component>>> componentMap; // sparse, TODO: type-erased handle?

	std::unordered_map<Handle<Entity>, std::string> entityName; // for editor
	//std::unordered_map<Handle<Entity>, enum Icon(?)> entityIcon; // for editor

public:
	virtual ~SceneBase() = 0;
};
```

씬은 기본적으로 하나만 메모리에 올라온 상태를 가정하지만, 오픈월드 같이 씬 스트리밍 기법을 활용해야 할 경우 복수의 씬이 메모리에 올라와 있을 수도 있다.
씬 매니저가 씬을 전환하면 다른 하부 관리자들은 씬에 적힌 대로 오브젝트들을 메모리에 올리거나 내린다.
각 매니저들은 자신들이 속한 월드(이벤트 시스템)를 거쳐 현재 활성화된 씬들을 알 수 있으며, 객체가 올라가거나 내려감으로써 참조관계가 갱신되는 것을 씬에 실시간으로 반영한다.
활성화된 씬은 씬 그래프를 비롯해 여러 매니저에게 보고받은 모든 참조관계의 정보를 항상 최신으로 유지하고 있어야 하며, 파일로서 씬이 저장될 때 컴포넌트 패러미터 초깃값을 싹 조사하여 씬에 굽는다.

퍼시스턴트 씬은 항상 활성화되어 플레이 중 계속 메모리에 상주하는 데이터들을 결정한다.
한 월드에는 퍼시스턴트 씬이 딱 하나 반드시 존재하며, 씬 매니저의 생성자에서 만들어진다.
퍼시스턴트 역시 씬 그래프를 가지며, 추가로 현재 메인씬의 핸들과 같은 게임 내 전역변수와 같은 존재들을 퍼시스턴트가 가진다.
게임(혹은 에디터도 하나의 게임으로 봐서)의 설정이나 씬이 전환되어도 유지되길 원하는 오브젝트들을 이 씬에 귀속시킨다.

메모리에 올라와 있는 기간이 길다.
따라서 씬의 용량은 매우 작고 스택기반으로 할당되며 캐시 최적화 가능성도 낮다.


### Entities

### Components

특정 수치를 데이터를 갖거나 리소스참조정보를 데이터로 갖는다.

에디터 모드에서는 인스펙터에 보이는 컴포넌트와 보이지 않는 컴포넌트가 구별됨
에디터를 통해 사용자가 컨트롤할 수 있는가의 여부임.




긴급한 문제상황.........
리소스 바인딩 시스템 vs 리소스 매니저
씬이 엔티티를 소유하고 엔티티가 컴포넌트를 소유하는 것은 (비교적)정적 상황이나, 컴포넌트가 리소스를 참조하는 것은 동적 상황이다. 따라서 엔티티매니저가 씬의 엔티티 소유를 관리하고 컴포넌트매니저가 엔티티의 컴포넌트 
소유를 관리하듯 리소스매니저가 컴포넌트의 리소스 참조를 관리하게 되면 앞으로 순환의존도, 확장성 및 교체유연성, 스트리밍/렌더 패스 꼬임, 멀티스레드 분리 어려움 등의 문제가 생기게 될 가능성이 높다. 챗지피티는 그래서 리소스바인딩시스템을 리소스 매니저와는 다르게 별도로 둘 것을 제안하고 있다.
메시컴포넌트는 바인딩시스템 외에 따로 접근하는 시스템이 없어서 메시컴포넌트의 구현위치를 고민해야 한다. 바인딩시스템 옆에 메시컴포넌트나 파티클컴포넌트나 스크립트컴포넌트 등을 두는 게 과연 맞을까?

스크립트 컴포넌트


렌더링과 관련된 컴포:

메시 컴포넌트
	gltf프리미티브+머티리얼 참조정보들의 리스트
	패러미터는 gltf액세서에도 있고, gltf머티리얼에도 있고, 머티리얼에도 있다
메시 컴포넌트의 데이터(리소스참조)는 누가 변경할 수 있지?
그건 렌더시스템이 아니라 게임로직시스템

스프라이트 컴포

파티클시스템 컴포넌트

ui요소







### Assets

assets vs resources

대충 내부 vs 외부, 최적화된 vs 원본


게임엔진에서 리소스는 단순히 데이터를 직접 소유하는 객체만 의미하지 않습니다.
즉 ModelResource 하나를 여러 컴포넌트가 동시에 참조하지만, 참조하는 데이터 영역과 목적은 서로 다릅니다.

#### custom format for scenes


#### custom asset format for gltf primitives?



#### custom asset format for materials
셰이더 코드는 애셋, 컴파일된 셰이더는 리소스.
머티리얼은 리소스, 그러나 이 리소스를 저장하면 애셋.

머티리얼은 다른 말로 셰이더 프로그램/패스의 래퍼라고 해도 된다. (셰이더가 들어가기에 gltf 내부 용어인 머티리얼과 충돌함에 주의)
심지어 고급 머티리얼은 복수의 패스를 가질 수도 있다.
좀 더 구체적으로는 드로우콜을 만들기 위해 필요한 셰이더 스테이트 묶음.

머티리얼 = 셰이더 프로그램 + 패러미터 세트 + 텍스처 바인딩

메시컴포가(혹은 프리미티브)가 지정한다. (다른 컴포들도 지정할 때 있음)

셰이더 코드 자체에 대한 참조정보가 기본.
셰이더 코드 내에 정의된 패러미터들(그냥 유니폼뿐만 아니라 다른 버퍼뷰 및 텍스처? 등 외부파일 참조정보 포함)의 사용자 지정 초깃값을 갖고 있음.
셰이더에 필요한 파일들, 즉 텍스처뿐만 아니라 그냥 바이너리 데이터도 참조함.





프리미티브(=서브메시) 하나에 버텍스셰이더 + 프래그먼트셰이더 지정 --> 드로우콜 하나
머티리얼은 메시/프리미티브와 이펙트에만 관여, 메시가 머티리얼 참조테이블 가짐


#### custom asset format for animations
애니메이션컴포가 지정.
애니메이션 파일(fbx이나 gltf, 정점좌표나 본변환(키프레임)값들)에 대한 참조정보가 기본.
이벤트 반응, 블렌딩 레이어/마스크, 리타겟, 압축 및 최적화 정보들을 포함.
본변환(키프레임)값, 루트 모션 정보도 갖고 있으면 최적화 용이.

#### custom asset format for effects
효과컴포가 지정.






### Resources

인스턴스를 위한　각종 설계도.
메모리에 한 번 올라오면 기본적으로 불변이지만 언로드 시에 인스턴스를 보고 변경해서 저장은 가능.
리소스 간 참조관계는 기본적으로 엉망진창이라 생각하자.

크게 api 의존 / api 비의존, 이렇게 두 개로 나뉜다.
전자는 buffer, texture, shader 같은 것들로 world에 있음.
후자는 mesh, model, material 처럼 컨테이너적 성격으로 각 시스템에 분산됨.



모델(gltf/glb), 정점(obj), 이미지(png), 오디오(wav)


gltf 모델 = 메시(프리미티브(버퍼/버퍼뷰/액세서 여럿 + 이미지/텍스처/머티리얼 하나)들) + 스킨? 애니메이션? 트랜스폼?

모델도 리소스, 머티리얼도 리소스, 셰이더도 리소스, 버퍼도 리소스, 텍스처도 리소스, 엔진 내부에서는 참조 및 의존관계가 완전 개판인데 그냥 다 리소스라 불러버리는 전략이 일반적

예를 들어 모델에 포함된 텍스처는 애셋 브라우저에 별도로 표시되지 않고, 이미지 딸랑 한장인 텍스처는 애셋 브라우저에 별도 파일로 표시되고 그런 느낌

엔진 내부 리소스로 관리되는 순간에는 “모델 내부에 포함된 텍스처인지, 독립 텍스처인지”는 사실상 중요하지 않음





## 2.3. Systems

스크립트/컴포넌트의 타입에 따라 어떤 시스템이 접근가능한지가 결정된다.

### Render


렌더시스템이 관리하는 컴포넌트들에 대해서
	1.	MeshRenderCache 실제 구조 예시 (CPU → GPU sync)
	2.	ParticleRenderCache + GPU Instancing 설계
	3.	DrawList 작성 파이프라인 (Visibility → Sort → Submit)



메시(컴포)인스턴스와 머티리얼(리소스?)인스턴스

리소스도 인스턴스도 GPUData를 가진다.

GPUData의 원칙: GPUData는 GPU에 올라간 데이터 핸들 + GPU-friendly draw info만 갖는다

### Collision

### Animation

### Visial effect

### Audio

### Input



# 3. Editor


Input: assets(prefabs, resources, secenes, scripts, etc.)
Output: executable, data(assets in runtime, cache, logs, etc.)


## Editor specific systems

씬 편집(계층, 인스펙터, 기즈모)

시각화(ui, camera)

에셋 관리(프로젝트, 브라우저, 아웃트로)

undo-redo

우선순위: ui, 계층, 인스펙터




<!--

## VisitorBase pattern for window rendering





dynamic_cast는 가상함수가 하나 이상 있어야지 작동하는구나...



“런타임에 타입이 결정된 컴포넌트들이 시스템별로 분리되어 저장되고, 시스템은 자신이 담당하는 타입만 업데이트하므로, 런타임 타입 체크는 최소화된다.”
이렇게 하면 비지터를 안 쓸 수 있게 됨.

Object: root
Module: depth one
Type: non-zero depth
Concrete: leaves

Component와 System은 (Module부터) 똑같은 상속계층구조를 가지게끔 하자!

Window manager가 이렇게 모든 Concrete Component별로 window render 함수를 분화시키는 것도 이상하다.










// Each concrete class actually inherits the CRTP class `Object` to implement the visit method.
/*
 * The virtual inheritance is used to cast the object to the correct type in run-time.
 * For instance, if a class `ModelResource` inherits from `Object` class and `Resource` class,
 * and if `Resource` class inherits from `Module` class, then in order to cast a pointer
 * `Resource* modelResource` that points to a `ModelResource` object, we can use
 * `dynamic_cast<Object*>(modelResource)` to get a pointer to the base class.
 * This is because `Object` is a virtual base class of `ModelResource`, allowing us to safely
 * down-cast it to the correct type.
 */

The VisitorBase pattern is one technique to achieve run-time polymorphism.
The accept method will be implemented within CRTP.

It is particularly useful for adding new operations to an existing object hierarchy without modifying the classes within that hierarchy (adhering to the Open/Closed Principle), which prevents scattering functionally similar but type-specific method implementations across various source files.

It also enables reference to unique member variables of concrete derived classes from a common interface pointer, which the usual virtual functions cannot provide.

An abstract CRTP base class that injects the implementation of the visit method to each object

/* The CRTPObject class is a CRTP base class that provides the implementation of the visit method.
 * It is used to avoid virtual inheritance and to provide a compile-time polymorphism.
 * The ObjectType is the concrete class that inherits from CRTPObject.
 * The ObjectType should be derived from Object, and it should not be abstract. */

/* (1) The class VisitorBase should be defined before this method, and forward declaration of VisitorBase is not sufficient. */



## 런타임타입정보로 멤버변수 접근하기
혹은 템플릿에 다형성...?


UI 렌더러는 매우 다양한 타입에 대응할 수 있어야 한다.
UI를 렌더링하는 함수를 대응해야 하는 타입의 각 클래스에 구현하는 것은 매우 비효율적이다. (Open/Closed Principle?)
따라서 UI 렌더러가 렌더링하는 함수를 각 타입별로 분화시키는 것이 바람직하다.
이때 타입정보는 런타임에 동적으로밖에 알 수 없고, 렌더링 함수는 각 타입의 멤버변수의 정보도 렌더링할 수 있어야 한다.

이때 사용할 수 있는 방법들을 정리해보자.

### 동적타입변환으로 경우 나누기

재귀적 구현이 가능한가?

### 방문자 패턴


### 런타임테이블
```
std::unordered_map<std::type_index, std::function<void(Base*)>>
```


```c++
std::unordered_map<std::type_index, Base> map;
for (const auto& [type, object] : map) {
	// do something with type and value
}
```
`type`이 가리키는 클래스 `Derived`가 추상클래스 `Base`의 파생클래스일 때, 런타임타입정보 `type`으로 순회하는 경우 `Base`가 가진 가상함수로 `Derived`의 멤버변수에 접근할 수 없다.


### 메타프로그래밍




## CRTP로 기능 주입하기

## 템플릿을 써야 할 때

1. 불특정 타입의 객체를 실제로 담는 컨테이너를 만들어야 할 때
2. 불특정 타입들을 위한 범용 유틸을 만들어야 할 때
3. 불특정 타입들이 공통된 기반클래스를 가질 수 없을 때





## Universal reference vs forwarding reference

허어어어어어어어어어어어

















# 하고 싶은 것

- 씬 전환
- 멀티스레드 파이프라인 구조로 점진적 전환


-->