| 시스템명        | 역할 요약               |
| ----------- | ------------------- |
| 렌더링 시스템     | 그래픽 처리 및 화면 출력      |
| 물리 시스템      | 물리 법칙, 충돌 처리        |
| 입력 시스템      | 유저 입력 감지 및 전달       |
| UI 시스템      | 사용자 인터페이스 구성 및 관리   |
| 애니메이션 시스템   | 캐릭터 및 객체 애니메이션 처리   |
| 오디오 시스템     | 사운드 재생 및 공간 음향      |
| 이벤트 시스템     | 이벤트 브로드캐스트 및 통신     |
| 스크립팅 시스템    | 게임 로직 제어            |
| 네트워크 시스템    | 멀티플레이어 데이터 송수신      |
| 리소스 관리 시스템  | 모든 리소스 로드, 캐시, 해제   |
| 씬 그래프/월드 관리 | 엔티티 관리, 공간 구조, 씬 조직 |



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





## Visitor pattern for window rendering

updater is introduced because window manager is not a system, but a manager for a module.
visitor pattern is introduced instead of overloading-based pattern in order to
- get run-time polymorphism for update method for example in `forEach<Module>(type, func)`,
- extend easily other types of objects to do something.


Double dispatch.

`void update(Updater& updater) override { updater.update(*this); }`


Since `Resource* resource`, we do not know the actual derived type of resource.
Then, `resource->update(*this)` involves dynamic dispatch bt virtual functions, and `updater.update(*this)` involves static dispatch by overloading.






dynamic_cast는 가상함수가 하나 이상 있어야지 작동하는구나...



“런타임에 타입이 결정된 컴포넌트들이 시스템별로 분리되어 저장되고, 시스템은 자신이 담당하는 타입만 업데이트하므로, 런타임 타입 체크는 최소화된다.”
이렇게 하면 비지터를 안 쓸 수 있게 됨.

Object: root
Module: depth one
Type: non-zero depth
Concrete: leaves

Component와 System은 (Module부터) 똑같은 상속계층구조를 가지게끔 하자!

Window manager가 이렇게 모든 Concrete Component별로 window render 함수를 분화시키는 것도 이상하다.