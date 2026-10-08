# Repository Guidelines

## 프로젝트 구조와 모듈 구성

ChatX는 단일 런타임 C++ 모듈로 구성된 Unreal Engine 5.5 프로젝트입니다. `Source/ChatX/` 아래의 `Game/`은 게임플레이 클래스, `Player/`는 플레이어 제어, `UI/`는 채팅 위젯을 담당합니다. 모듈 의존성은 `ChatX.Build.cs`, 게임 및 에디터 빌드 타깃은 `Source/*.Target.cs`에서 관리합니다.

`Content/ChatX/Blueprint/`에는 GameMode, PlayerController, UI 에셋이 있습니다. 시작 맵은 `Content/ChatX/Maps/Chatting.umap`이며, 공통 프로젝트 설정은 `Config/Default*.ini`에 있습니다. `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `.vs/`는 로컬에서 생성되는 결과물입니다.

## 빌드와 로컬 실행

UE 5.5와 `.vsconfig`에 명시된 Visual Studio C++ 및 Windows SDK 구성 요소를 설치하세요. 필요하면 `ChatX.uproject`에서 Visual Studio 프로젝트 파일을 생성하고, `ChatX.sln`을 **Development Editor / Win64** 구성으로 빌드하세요.

PowerShell에서는 `$UE55`를 엔진 설치 경로로 지정한 뒤 프로젝트 루트에서 실행하세요.

```powershell
& "$UE55\Engine\Build\BatchFiles\Build.bat" ChatXEditor Win64 Development "${PWD}\ChatX.uproject" -WaitMutex
& "$UE55\Engine\Binaries\Win64\UnrealEditor.exe" "${PWD}\ChatX.uproject"
```

첫 번째 명령은 에디터 타깃을 빌드하고, 두 번째 명령은 프로젝트를 엽니다. 채팅 동작은 에디터의 Play In Editor에서 확인하세요.

## 코드 스타일과 이름 규칙

기존 C++ 및 C# 코드에 맞춰 탭으로 들여쓰기하고, 중괄호는 별도 줄에 작성하며, 이름에는 PascalCase를 사용하세요. `ANBPlayerController`, `UNBChatInput`처럼 Unreal 타입 접두사를 유지하고, 클래스 헤더와 `.cpp` 파일의 이름을 일치시키세요. `.generated.h`는 마지막 헤더 include로 두세요. 리플렉션 대상에는 Unreal 매크로를 사용하고, 리플렉션 대상 객체 참조에는 `TObjectPtr`를 사용하세요.

Blueprint 접두사 `BP_`, `WBP_`를 유지하세요. 바인딩된 위젯 이름은 `EditableTextBox_ChatInput`처럼 C++ 프로퍼티 이름과 일치해야 합니다. 저장소에 별도 포매터나 린트 설정은 없습니다.

## 테스트 지침

현재 자동화 테스트와 커버리지 기준은 없습니다. 변경 후 에디터 타깃을 빌드하고 `Chatting` 맵을 실행해 다음을 확인하세요.

- 채팅 입력 위젯이 표시되는지 확인합니다.
- Enter 입력 시 메시지가 출력되고 입력란이 비워지는지 확인합니다.
- Enter 이외의 텍스트 확정 동작에서는 메시지가 제출되지 않는지 확인합니다.
- Output Log에 오류가 없는지 확인합니다.

자동화 테스트를 추가할 때는 `Source/ChatX/Tests/`에 Unreal Automation Tests를 작성하고, `ChatX.UI.ChatInput`처럼 기능을 나타내는 이름을 사용하세요.

## 커밋과 Pull Request 지침

현재 작업 폴더에는 Git 메타데이터가 없어 기존 커밋 관례를 확인할 수 없습니다. `Fix chat input submission`처럼 변경 내용을 나타내는 간결한 명령형 제목을 사용하세요. PR에는 동작 변경 설명, 관련 이슈 링크, 빌드 및 검증 결과를 포함하세요. UI 변경에는 스크린샷을 첨부하고, 수정한 Blueprint 에셋을 명시하세요. 생성된 로컬 결과물은 커밋에서 제외하세요.
