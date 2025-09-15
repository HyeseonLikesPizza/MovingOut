# Moving Out

###### 개발 기간: 2025.08.04 ~ 2025.09.01

간단한 이사(운반) 콘셉트의 UE5 프로젝트입니다. 트럭과 다양한 오브젝트(문, 창문, 골대, 차고 등)를 활용해 목표를 달성하는 게임 플레이를 실험합니다. C++과 블루프린트가 혼합된 구조로, UI/게임모드/캐릭터/프롭 시스템을 포함합니다.

## 엔진 및 개발 환경
- Unreal Engine: 5.5 (`EngineAssociation: 5.5`)
- 프로젝트 타입: C++ + 블루프린트 혼합
- 지원 OS: Windows (기본 개발 환경)
- 필수 툴: Visual Studio 2022(또는 호환 버전), .NET/Windows SDK, Unreal Engine 5.5

## 주요 기능(요약)
- 캐릭터/트럭 조작 및 상호작용 컴포넌트(`InteractiveComponent`)
- 목표 구역/카운트 오브젝트 등 프롭 시스템(`PropsBase`, `PropsGoalZone`, `CountProps` 등)
- 파괴 가능한 오브젝트(창문, 가로등 등)
- 간단한 AI/스플라인 차량(`AISplineCar`)
- 게임 모드/게임 인스턴스/게임 스테이트 관리
- UI 시스템(메인 메뉴, 타이틀, 스테이지 선택, 오버레이, 결과 화면 등)
- 게임플레이 태그(`MovingOutGameplayTags`)

## 폴더 구조(발췌)
- `Source/MovingOut`
  - `Character/` 플레이어, 적, 트럭 관련 C++ 코드
  - `Component/` 상호작용 등 공용 컴포넌트
  - `Controller/` 플레이어/디버그 컨트롤러
  - `Game/` GameMode, GameState, GameInstance
  - `Props/` 파괴/카운트/문/골대/차량 등 프롭 로직
  - `UI/` 위젯, 컨트롤러, 서브시스템
  - `Data/` 설정/밸런스 에셋 관련 클래스
- `Content/Blueprints/` 블루프린트 위젯, 캐릭터, 트럭, AI 등
- `Config/` 프로젝트 기본 설정 (`DefaultEngine.ini`, `DefaultGame.ini`, `DefaultInput.ini` 등)

## 의존성 및 플러그인
- 모듈 의존성: `CoreUObject`, `Engine`, `CableComponent`
- 사용 플러그인: `ModelingToolsEditorMode`(에디터), `HDRIBackdrop`, `PostProcessMaterialChainGraph`

## 빌드 및 실행
1) UE 5.5 설치 후, 프로젝트 루트의 `MovingOut.uproject` 우클릭 → `Generate Visual Studio project files` 실행
2) 생성된 솔루션(`MovingOut.sln`)을 Visual Studio로 열고 `Development Editor | Win64` 구성에서 빌드
3) 에디터에서 `Play`로 테스트하거나, `File > Package Project > Windows`로 패키징

참고: 처음 열 때 셰이더/Derived Data Cache 빌드로 인해 시간이 소요될 수 있습니다.

## 입력/조작
- Enhanced Input 기반으로 설정되어 있습니다.
- 트럭 입력 관련 예시 에셋: `Content/Blueprints/Characters/Truck/Input/IMC_Truck`, `IA_TruckMoveForward`, `IA_TruckTurn`
- 실제 키/패드 매핑은 `DefaultInput.ini` 및 IMC/IA 에셋에서 확인해 주세요.

## 개발 메모/로드맵(예시)
- 기본 스테이지 클리어 조건 정리 및 UI 연동 보강
- 프롭 상호작용 연출(사운드/파티클) 확장
- 트럭/플레이어 조작감 튜닝 및 카메라 워크 개선
- 저장/로딩(설정, 진행도) 추가 검토

## 기여/문의
- 버그 리포트나 제안 사항은 이슈로 남겨 주세요.
- 협업을 원하시면 브랜치/PR 가이드에 맞춰 주세요.
