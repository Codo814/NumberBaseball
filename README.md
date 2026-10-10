# NumberBaseball

Unreal Engine 5.5와 C++로 만드는 멀티플레이 채팅 숫자야구 학습 프로젝트입니다.

## 현재 구현

- 채팅 입력을 Server RPC로 전달하고 Client RPC로 결과 출력
- 서버에서 1~9 중 서로 다른 세 자리 정답 생성
- 서로 다른 세 자리 숫자 입력의 스트라이크·볼·OUT 판정
- 숫자야구 입력 이외의 메시지는 일반 채팅으로 처리

시도 횟수 관리와 승패·게임 리셋은 아직 구현하지 않았습니다.

## 실행

1. Unreal Engine 5.5와 Visual Studio C++ 개발 도구를 설치합니다.
2. `NumberBaseball.uproject`에서 Visual Studio 프로젝트 파일을 생성합니다.
3. `NumberBaseballEditor`를 Development Editor / Win64로 빌드합니다.
4. 에디터에서 `Content/ChatX/Maps/Chatting` 맵을 엽니다.
5. 멀티플레이 PIE에서 채팅과 판정 결과를 확인합니다.

## 구성

- `Source/NumberBaseball/Game`: 서버 판정과 게임 상태
- `Source/NumberBaseball/Player`: 플레이어 컨트롤러와 RPC
- `Source/NumberBaseball/UI`: 채팅 입력 위젯
- `Content`: Blueprint와 맵
- `Config`: 프로젝트 설정

TeamSparta 강의 내용을 바탕으로 진행 중인 학습 프로젝트입니다. 별도 라이선스는 부여하지 않았습니다.