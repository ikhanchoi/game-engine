

I used Chatgpt with copying and pasting codes fewer than 10 lines for convenience.



# Game
Input: assets(prefabs, resources, secenes, scripts, etc.)
Output: executable, data(assets in runtime, cache, logs, etc.)


# Objects

Something that reference relations and lifecycle management should be clear.

Basically ownership is not used, but reference relation is used to manage all lifecycles.

World -> Scene/Persistent -> Prefab/Entity -> Script/Component(Parameter) -> Resource/Primitive(Data)



파라미터란 컴포넌트가 가지는 변수값과 리소스의 경로를 말한다.
데이터란 리소스파일 내부의 값을 말한다.
파라미터와 데이터 모두 메모리에 올라온 상황에서는 매우 빠른 속도로 변경될 수 있다.
따라서 캐시친화적 최적화 대상이다.



### Managers
객체들을 관리하는 신의 사자와 같은 존재들이며, 월드가 소유한다.
구체적으로, 관리자는 임포터와 할당자, 참조카운터 유틸을 이용함으로써 메모리 위 객체의 생성과 소멸을 담당한다.
씬이 전환되는 상황에서는 생성과 소멸이라기보다는 객체의 활성화 및 비활성화가 이미지적으로는 더 알맞을 것이다.
물론 이 객체의 활성화는 메모리에서의 생성 직후 초기화작업이 붙은 경우를 의미한다.
관리자는 관리자가 살아 있는 동안 씬이 전환됨으로써 객체들이 얼마든 새로 활성화되거나 비활성화될 수 있다는 뜻에서 객체지향적인 의미로 담당하는 객체들의 수명(분량)을 책임지지 않는다.
객체가 메모리에 올라와 있지 않아도 우리는 그 객체가 비활성화된 채 존재한다고 인식할 수 있으며, 관리자가 그 객체를 메모리(무대, 월드)에 올리고 내리는 역할만 할 뿐이다.
이런 의미에서 객체들은 기본적으로 어딘가에 소유되지 않는다.

관리자들끼리 서로 참조를 할 수 있다.
씬 관리자가 씬을 전환하면 다른 하부 관리자들은 씬에 적힌 대로 객체들을 올리거나 내린다.
시스템은 관리자에 의해 활성화된 객체들에만 접근하여 값을 갱신하거나 참조하거나 반환할 수 있다. 
각 매니저들은 자신들이 속한 월드를 거쳐 현재 활성화된 씬들을 알 수 있으며, 객체가 올라가거나 내려감으로써 참조관계가 갱신되는 것을 씬에 실시간으로 반영한다.


무대 밖에 존재하던 배우가 무대 위로 올라오거나 내려가는 것 말고도, 정말로 무대 위에서 배우가 태어나고 죽어서 존재 자체가 사라질 수 있다.
씬이 끝나면 모든 것이 초기화되어 태어났던 배우는 없어지고 죽었던 배우는 살아난다.




관리자는 SoA로 데이터를 할당해 컴포넌트간 상호참조 시에 캐시친화적 최적화를 도모한다.




### Importers
- 리소스: 모델 임포터, 셰이더 컴파일러
- 스크립트: 스크립트 컴파일러, 멤버변수 정보 불러온다든가
- 프리팹 임포터
- 씬 로더


매니저에 의해 객체를 생성할 때 호출된다.


### Allocators


관리자는 각 활성화된 객체타입이 정확히 어떤 할당자에 의해 할당됐는지 몰라도 좋다.
시스템이 활성화된 객체를 순회할 때, 해당 타입이 정확히 어떤 할당자에 의해 할당됐는지 알아야 한다. 왜냐 하면 타입을 모르는 함수객체를 forEach에 넘겨줄 수 없기 때문이다.




## Worlds

메모리에 올라온 패러미터 및 데이터와 시스템 사이의 상호작용가능한 것들을 활성화시킨다.
구체적으로, 객체들을 활성화하고 시스템을 업데이트시킨다.
극장(Editor/Player)에 의해 소유되어, 활성화된 장면(Scene)에 지시된 대로 배우(Entity)들이 등장해 규칙에 맞추어 움직이는 무대.
객체(할당자에 의해 할당되는 대상)가 아닌 것으로 이해하자.

에디터는 에디트월드와 플레이월드, 기본적으로 총 두 개의 월드를 소유할 수 있다.
에디트월드는 에디터의 생성자와 소멸자에서 생성되고 소멸된다.
플레이월드는 플레이모드에 진입하고 퇴장했을 때 생성되고 소멸된다.
플레이어는 플레이월드 하나만을 소유한다.
나중에 멀티플레이게임을 구현하게 되면 월드가 훨씬 많아질 수 있다.



월드는 기본적으로 전역적으로 정의할 만한 것들을 유니크포인터로 소유한다고 생각하자.
관리자들과 시스템들, 그리고 코어의 임포터들과 할당자들의 인스턴스들을 소유한다.
한 월드에서는 퍼시스턴트 하나, 그리고 프로젝트가 가진 여러 씬들 중 하나, 이렇게 두 개가 항상 활성화되어 있다.
활성화된 씬도 게임 시작 씬으로서 초기화가 되어 있어야 한다.
월드의 책임은 프레임마다 시스템을 호출하는 것과 필요 시 스택할당자로 씬의 활성상태를 전환하는 것이다.

관리자는 할당자와 참조카운터 유틸을 가지고 객체의 (메모리에서의) 생성과 소멸을 담당한다.
시스템은 할당된 패러미터와 데이터의 값을 참조하거나 갱신한다.


## Persistents

퍼시스턴트는 항상 활성화되어 플레이 중 계속 메모리에 상주하는 데이터들을 결정한다.
퍼시스턴트 역시 엔티티, 컴포넌트, 리소스의 참조정보 및 컴포넌트와 리소스의 데이터 초깃값를 갖는다.

게임 전체에 존재하는 보편적인 씬이라 생각하면 된다.



## Scenes


씬은 활성화됨으로써 메모리에 올라와 상호작용이 가능해지는 데이터들을 결정한다.
씬의 소유권은 프로젝트가 갖고 있으며, 활성화와 비활성화만 월드가 책임진다.
에디트월드에서는 씬의 활성화여부가 씬의 내용 변경가능 여부 또한 의미한다.

씬은 엔티티 및 프리팹의 씬그래프, 엔티티(프리팹)-컴포넌트(스크립트) 참조 테이블을 인스턴스로서 갖는다. 

씬은 자신이 소유한 엔티티 그래프 정보와 엔티티 및 컴포넌트 사이의 소유관계들을 책임지지만, 컴포넌트가 리소스를 참조하는 것은 컴포넌트의 패러미터로서 이해된다는 것을 기억하자.
따라서 리소스가 컴포넌트에 부착되거나 변경되는 것은 해당 컴포넌트의 타입을 책임지는 시스템이나 매니저에 의해 이루어진다.
활성화된 씬은 엔티티 트리를 비롯해 관리자에게 보고받은 모든 참조관계의 정보를 항상 최신으로 유지하고 있어야 하며, 파일로서 씬이 저장될 때만 컴포넌트 패러미터 초깃값을 싹 조사하여 씬에 구워진다.

메모리에 올라와 있는 기간이 길다.
따라서 씬의 용량은 매우 작고 스택기반으로 할당되며 캐시 최적화 가능성도 낮다.

하나의 월드에는 플레이 중 기본적으로 퍼시스턴트를 제외한 최대 하나의 씬만이 활성화될 수 있다고 생각하면 좋지만, 여러 씬이 활성화되는 것도 불가능한 것은 아니다.
씬이 활성화된다는 것은 그 씬이 참조하는 파라미터와 데이터가 메모리, 즉 스토리지에 올라온다는 뜻이다.
반대로 어떤 데이터가 메모리에 올라오려면 그 데이터를 참조하는 씬이 활성화되어야 한다.
씬이 활성화되어 메모리에 올라온 파라미터와 데이터는 상호작용으로 값이 변경될 수 있다.
씬이 비활성화되면 파라미터와 데이터는 메모리에서 해제되고 플레이 중 변경된 값은 잊힌다.
씬이 다시 활성화되면 컴포넌트와 리소스는 초깃값을 가지고 다시 메모리에 올라온다.
만약 파라미터나 데이터가 지속되길 원한다면 해당 컴포넌트나 리소스는 퍼시스턴트가 참조해야 한다.


플레이월드에서 씬은 변경될 수 없지만 에디트월드에서는 씬의 내용을 변경할 수 있는 시스템들이 다수 존재한다.
씬은 매니저에 의해 관리되지 않지만 에디트월드에서 




## Components



## Scripts

스크립트는 다음과 같은 일들을 해야 한다.
- 지연변수로서 커스텀 패러미터들을 정의한다.
- 커스텀 이벤트 디스패처들을 정의한다.
- 

시스템은 각 이벤트 디스패처가 어떤 구독자들을 갖고 있는지에 대한 테이블을 갖고 있다.


스크립트는 시스템들을 구독자들을 등록해야 한다.




# Systems

시스템은 다음과 같은 메소드들을 매 프레임 호출한다.

- event collector
- event handler (콜백, 진입, 스크립트에서 구현, 커맨드빌더를 호출, On으로 시작하는 이름 많음)
- command builder (이벤트핸들러가 호출, 일반적인 메소드 이름)
- command executor

총 네 층위의 메소드를 가진다. 이 네 층위는 한 루프의 4등분이기도 하다. 이벤트핸들과 커맨드생성을 생략하여 여러 시나리오를 디자이너에게 노출되지 않게 할 수도 있다.

두 개의 일을 한다.
1. detection: 매 프레임마다 스크립트/컴포넌트들을 돌며 일일이 이벤트를 판단해서 이벤트 객체들을 모은다
2. 이 이벤트에 맞게 시스템에 미리 정의된 이벤트 발신용 콜백(=이벤트 핸들러)을 이벤트 객체들마다 호출하고, 이 콜백에 반응하는 스크립트/컴포넌트에 등록된(커스텀 이벤트 디스패처들이 소유한) 구독자 리스트를 통해 이벤트를 커맨드(들)로 변환해 시스템 내부의 커맨드큐에 쌓는다.
3. resolution: 매 프레임마다 커맨드큐를 처리하여 스크립트/컴포넌트, 리소스/프리미티브의 파라미터 및 데이터의 값을 업데이트하거나 값을 참조하여 일을 한다.

이때 일을 하는 구체적인 알고리즘들도 시스템 내부에 구현되어 있다.
리소스/프리미티브의 데이터가 바뀌는 경우는 물리, 애니, 에디터 내 수정툴 정도 말고는 거의 없는 특수한 상황이기 때문에, 시스템은 주로 스크립트/컴포넌트의 파라미터 값을 참조하거나 갱신하는 일을 한다.

중앙스케줄러는 일을 처리함과 동시에 undo-redo 스택에도 일을 기록한다.




스크립트/컴포넌트의 타입이 어떤 시스템에 반응해야 하는지를 판단한다.

## Render
컬링 최적화 포함?

## Collision

## Animation

## Visial effect

## Audio

## Input


인풋시스템은 GLFW에서 콜백이나 폴링으로 받은 인풋을 이벤트화하여 

구체적인 해야 할 일이 적힌 커맨드로 바꾸어 각 시스템이나 매니저로 뿌리는 일?, 즉 이벤트시스템인가?

Command pattern...

### Event vs Command vs Task
Event: 무언가가 발생했다는 사실을 알림
- 'Ctrl + C' 키가 눌렸다
- 마우스 왼쪽 버튼이 눌렸다
- 충돌이 발생했다
- 애니메이션이 끝났다

Command: 무언가를 하라는 지시
- 'Ctrl + C' 키가 눌렸으니 복사해라
- 마우스 왼쪽 버튼이 눌렸으니 오브젝트를 선택
- 충돌이 발생했으니 반응해라
- 애니메이션이 끝났으니 다음 애니메이션 재생

Task: 주로 비동기적이나 지연적 작업을 처리하기 위한 단위. 코루틴, 리소스 로딩, 게임 루프에서의 예약 실행?

processEvent는 인풋시스템이나 충돌시스템이 이벤트를 받아서 커맨드로 바꾸는 일이다.
해당 일을 해야 하는 시스템이나 매니저에 커맨드를 보내는 일이다.
만약 비동기적인 처리를 해야 하는 커맨드의 경우, 태스크라는 래퍼로 감싸서

executeCommand: 커맨드를 받아서 실제로 무언가를 하는 일


# Editor


## Editor specific systems

씬 편집(계층, 인스펙터, 기즈모)
시각화(ui, camera)
에셋 관리(프로젝트, 브라우저, 아웃트로)
undo-redo, 


우선순위: ui, 계층, 인스펙터, 

# Debugger






---

---

##

외부 구현: 생성자, 소멸자
내부 구현: setter, getter

기본적으로 type_index를 인자로 받는 메소드는 find !end 예외처리 고려할 것


unique_ptr 멤버: 외부 공유할 생각 없을 때




## 코어 클래스들


### Handle
weak_ptr를 대신함
논리적으로 반드시 필요하다기보다는 성능(카운팅, 지역성)과 타입 및 스레딩 안정성 면 등 여러 모로 유리하기 때문에 도입
lock() 개념도 없애서 매니저를 통해서만 접근이 가능하도록 제한하여 설계하였음


### Manager
create과 destroy만 담당, 개별 setter/getter 없지만 forEach와 access가 있음
forEach는 pool 정보를 갖고 pool 내를 포문 돎.
system과 다르게(?) 직접 


## 메소드 네이밍

### register
type_index를 이용한 동적 작동 금지, 템플릿을 이용한 정적 작동만 허용하는 void 메소드를 위한 네이밍.
예를 들어 컨텍스트가 매니저 등록할 때와 매니저가 타입 등록할 때.
팩토리로 동적으로 생성하지 않음.

### access
스마트포인터로 관리되고 있는 것들의 로포인터를 반환. 수명신경쓰지 않아도 되는 단순 참조용이지만 handle을 반환하는 경우보다 훨씬 강력.

### get
매니저를 겟할 땐 스마트포인터로 관리되고 있는 것들을 dynamic_pointer_cast 없는 weak_ptr로 반환.
오브젝트를 겟할 땐 직접 관리되고 있는 것들의 handle 반환.
동적 정적 둘 다 만들 것.

그외에 아이디나 이름이나 타입리스트와 같은 것들 반환.

### add
매니저가 create하면서 access로 초기화하고 get으로 handle 반환.
register와 달리 이름이 필요한 오브젝트들에게 사용.
매니저는 add하지 않음.
동적 정적 둘 다 만들 것.

### load
add로 create할 때 초기화용 함수들


##

### Module

static generateManager()가 존재하는 Object의 자식 클래스들
아직 requires 등은 구현하지 않음

### Type (Concrete)

static getTypeName()가 존재하는 Object의 자식 클래스들

## 소유관계

Entity





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