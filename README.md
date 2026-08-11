# HW6 - 멀티플레이 숫자 야구

Unreal Engine 5 C++로 구현한 서버 권한 기반 멀티플레이 숫자 야구 게임입니다.

클라이언트는 숫자 입력만 서버에 요청하고, 입력 검증·시도 횟수·S/B/OUT 판정·승패·턴·타이머는 서버가 결정합니다. 서버에서 변경된 상태는 RPC와 Replication을 통해 모든 플레이어의 UMG에 반영됩니다.

## 개발 환경

| 항목 | 내용 |
| --- | --- |
| Engine | Unreal Engine 5.5 |
| Language | C++ |
| UI | UMG (`WBP_MainWidget`) |
| Network Test | PIE Listen Server, 2 Players |
| Project Module | `HW6` |

## 게임 규칙

- 서버는 1~9 사이의 중복되지 않는 숫자 3개를 정답으로 생성합니다.
- 숫자와 위치가 모두 같으면 Strike입니다.
- 숫자는 존재하지만 위치가 다르면 Ball입니다.
- 일치하는 숫자가 하나도 없으면 OUT입니다.
- 올바른 형식의 입력 또는 턴 시간 초과 시 시도 횟수가 1회 증가합니다.
- 각 플레이어는 라운드마다 최대 3회까지 시도할 수 있습니다.
- 먼저 3 Strike를 달성한 플레이어가 승리합니다.
- 모든 플레이어가 기회를 소진하면 무승부입니다.
- 승리 또는 무승부 공지 3초 후 새 정답과 함께 다음 라운드를 시작합니다.

입력은 다음 조건을 모두 만족해야 합니다.

- 정확히 3자리
- 숫자만 포함
- 0 제외
- 중복 숫자 제외

잘못된 입력은 시도 횟수를 소진하지 않으며 현재 턴도 유지됩니다.

## 구현 기능

### 필수 기능

- [x] `GameModeBase` 기반 서버 판정 로직
- [x] 1~9 사이의 중복 없는 3자리 난수 생성
- [x] 3자리·숫자·0·중복 입력 검증
- [x] Strike, Ball, OUT 판정
- [x] `PlayerState` 기반 플레이어별 시도 횟수 관리
- [x] 시도 횟수 Replication 및 UMG 갱신
- [x] Server RPC를 통한 클라이언트 입력 전달
- [x] `/chat 내용` 형식의 기본 멀티플레이 채팅
- [x] Client RPC를 통한 개인 오류 메시지 전달
- [x] NetMulticast RPC를 통한 전체 판정·결과 공지
- [x] 승리 및 무승부 판정
- [x] 결과 공지 중 추가 입력 차단
- [x] 3초 후 시도 횟수·정답·UI 자동 리셋
- [x] C++ 기반 UMG 로직과 Widget Blueprint 연동

### 도전 기능

- [x] 접속 순서에 따른 `Player 1`, `Player 2` 이름 지정
- [x] 서버 권한 기반 턴 제어
- [x] 현재 턴 PlayerState Replication
- [x] 서버에서 관리하는 10초 턴 타이머
- [x] 모든 클라이언트의 남은 시간 동기화
- [x] 현재 턴 플레이어만 입력 가능
- [x] UI 입력 차단과 서버 측 이중 검증
- [x] 시간 초과 시 기회 1회 소진
- [x] 정상 입력 또는 시간 초과 후 다음 플레이어로 턴 전환
- [x] 기회를 모두 소진한 플레이어 자동 건너뛰기
- [x] 현재 턴 플레이어 퇴장 시 다음 턴 처리

## 클래스별 책임

| 클래스 | 실행 위치 | 역할 | 주요 기능 |
| --- | --- | --- | --- |
| `AHW6GameMode` | 서버 전용 | 전체 게임 규칙의 권위자 | 정답 생성, 입력 검사, 판정, 승패, 턴, 타이머, 리셋 |
| `AHW6GameState` | 서버 + 모든 클라이언트 | 전체 공유 상태 | 현재 턴과 남은 시간 복제, 전체 메시지 Multicast |
| `AHW6PlayerState` | 서버 + 모든 클라이언트 | 플레이어별 공유 상태 | 플레이어 이름, 현재/최대 시도 횟수 |
| `AHW6PlayerController` | 서버 + 소유 클라이언트 | 입력과 네트워크 통신 | 숫자와 채팅 분기, Server RPC, Client RPC, 로컬 UI 갱신 |
| `UHW6MainWidget` | 로컬 클라이언트 | 화면 표시와 입력 전달 | 버튼·Enter 입력, 메시지·횟수·턴·타이머·결과 표시 |
| `WBP_MainWidget` | 로컬 클라이언트 | 실제 UMG 레이아웃 | C++ `BindWidget` 대상 위젯 배치 |

### 클래스를 분리한 이유

- 비밀 정답과 판정 규칙은 클라이언트에 존재하지 않는 `GameMode`에 보관합니다.
- 모든 플레이어가 알아야 하는 현재 턴과 타이머는 `GameState`에 복제합니다.
- 플레이어마다 독립적인 시도 횟수는 `PlayerState`가 소유합니다.
- 클라이언트가 소유하는 `PlayerController`를 Server RPC의 진입점으로 사용합니다.
- Widget은 게임 규칙을 직접 판단하지 않고 입력 전달과 결과 표현만 담당합니다.

## 전체 네트워크 흐름

```text
WBP_MainWidget 숫자 입력
    ↓
UHW6MainWidget::HandleSubmitClicked
    ↓
AHW6PlayerController::SubmitGuess
    ↓ Server RPC
AHW6PlayerController::ServerSubmitGuess_Implementation
    ↓
서버 AHW6GameMode::ProcessPlayerInput
    ├─ 라운드 상태 검사
    ├─ 현재 턴 검사
    ├─ 남은 시간과 기회 검사
    ├─ ValidateInput
    ├─ AHW6PlayerState::AddAttempt
    └─ CheckAnswer
        ↓
AHW6GameState NetMulticast
    ↓
각 클라이언트의 AHW6PlayerController
    ↓
WBP_MainWidget 갱신
```

클라이언트가 서버에 보내는 값은 입력 문자열뿐입니다. 서버는 RPC를 받은 PlayerController를 통해 실제 요청자의 PlayerState를 찾기 때문에 클라이언트가 다른 플레이어나 시도 횟수를 임의로 지정할 수 없습니다.

## RPC와 Replication 사용 기준

| 방식 | 사용 위치 | 이유 |
| --- | --- | --- |
| Server RPC | `ServerSubmitGuess` | 소유 클라이언트가 서버에 입력을 요청 |
| Server RPC | `ServerSubmitChatMessage` | 소유 클라이언트가 서버에 채팅 전송을 요청 |
| Client RPC | `ClientReceiveMessage` | 잘못된 입력 등 요청자 개인에게만 오류 전달 |
| NetMulticast RPC | 판정, 시간 초과, 승리·무승부 공지 | 서버에서 발생한 사건을 전원에게 전달 |
| Property Replication | 시도 횟수, 현재 턴, 남은 시간 | 모든 인스턴스가 지속 상태의 최신값을 유지 |

`CurrentAttempts`, `CurrentTurnPlayer`, `RemainingTurnSeconds`는 `ReplicatedUsing`을 사용합니다. 클라이언트에서 값이 갱신되면 OnRep 함수가 PlayerController에 UI 새로고침을 요청합니다.

OnRep는 서버에서 자동 호출되지 않으므로 Listen Server의 로컬 UI를 위해 서버의 Setter에서도 같은 갱신 함수를 호출합니다.

## 주요 구현 과정

### 1. 게임 클래스 연결

`AHW6GameMode` 생성자에서 프로젝트가 사용할 클래스를 지정했습니다.

```cpp
AHW6GameMode::AHW6GameMode()
{
    GameStateClass = AHW6GameState::StaticClass();
    PlayerControllerClass = AHW6PlayerController::StaticClass();
    PlayerStateClass = AHW6PlayerState::StaticClass();
}
```

`DefaultEngine.ini`에는 `GlobalDefaultGameMode=/Script/HW6.HW6GameMode`를 지정했습니다.

### 2. 정답 생성

`BeginPlay()`에서 `GenerateRandomNumbers()`를 호출합니다. 후보 숫자를 생성한 뒤 `TArray::Contains()`로 중복을 검사하며 배열 크기가 3이 될 때까지 반복합니다.

정답은 `GameMode`의 `TArray<int32> SecretNumbers`에만 저장하며 복제하지 않습니다. 개발 중 판정 확인을 위해 서버 Output Log에는 정답을 출력합니다.

### 3. 입력 검증

`ValidateInput()`의 처리 순서는 다음과 같습니다.

1. 앞뒤 공백 제거
2. 문자열 길이가 3인지 검사
3. 각 문자가 숫자인지 검사
4. 0 포함 여부 검사
5. 이미 등장한 숫자인지 검사

검증에 성공한 경우에만 정리된 입력을 반환합니다. 실패하면 오류 메시지를 Client RPC로 요청자에게 보내고 `AddAttempt()`를 호출하지 않습니다.

### 4. S/B/OUT 판정

`CheckAnswer()`는 입력 문자열의 각 문자를 정수로 변환해 정답 배열과 비교합니다.

```cpp
if (SecretNumbers[Index] == GuessDigit)
{
    ++OutStrikeCount;
}
else if (SecretNumbers.Contains(GuessDigit))
{
    ++OutBallCount;
}
```

`else if`를 사용해 Strike인 숫자가 Ball로도 중복 계산되는 것을 막았습니다. Strike와 Ball이 모두 0이면 `OUT`, 그 외에는 `1S2B` 형식으로 반환합니다.

### 5. 시도 횟수 관리

`AHW6PlayerState`는 `CurrentAttempts`와 `MaxAttempts`를 관리합니다.

```cpp
UPROPERTY(ReplicatedUsing = OnRep_CurrentAttempts)
int32 CurrentAttempts;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
int32 MaxAttempts;
```

`AddAttempt()`와 `ResetAttempts()`는 `HasAuthority()`를 검사해 서버에서만 값을 변경합니다. `CurrentAttempts`는 `DOREPLIFETIME`으로 등록해 클라이언트에 복제합니다.

### 6. 승리, 무승부와 리셋

- `StrikeCount == SecretNumberLength`이면 해당 플레이어가 승리합니다.
- `GameState->PlayerArray`의 모든 `AHW6PlayerState`가 기회를 소진하면 무승부입니다.
- `EndRound()`에서 `bRoundActive`를 false로 바꾸고 턴 타이머를 중지합니다.
- `ResultOverlay`를 모든 클라이언트에 표시합니다.
- `FTimerManager`로 3초 후 `ResetGame()`을 호출합니다.
- `ResetGame()`은 모든 시도 횟수를 0으로 만들고 새 정답과 새 턴을 생성합니다.

### 7. 턴과 타이머

`StartTurn()`은 현재 턴 플레이어, 활동 여부, 남은 시간을 초기화하고 1초 반복 타이머를 시작합니다.

```cpp
CurrentTurnPlayer = TurnPlayer;
bSubmittedThisTurn = false;
RemainingTurnSeconds = TurnDurationSeconds;
```

`HandleTurnTimerTick()`은 서버의 남은 시간을 1초씩 감소시켜 `GameState`에 반영합니다. 시간이 0이 되면 `HandleTurnTimeout()`이 해당 플레이어의 시도 횟수를 증가시키고 `AdvanceTurn()`을 호출합니다.

`AdvanceTurn()`은 Unreal의 `PlayerArray`를 원형으로 순회하며 `HasAttemptsLeft()`가 true인 다음 PlayerState를 찾습니다. 기회를 모두 소진한 플레이어는 건너뜁니다.

### 8. UMG 연결

`UHW6MainWidget`은 `BindWidget`으로 `WBP_MainWidget`의 구성 요소와 연결됩니다.

| Widget 이름 | 타입 | 표시 내용 |
| --- | --- | --- |
| `GuessInputTextBox` | Editable Text Box | 숫자 또는 `/chat 내용` 입력 |
| `SubmitButton` | Button | 서버에 입력 제출 |
| `MessageTextBlock` | TextBlock | 오류 및 판정 메시지 |
| `AttemptsTextBlock` | TextBlock | 현재/최대 시도 횟수 |
| `CurrentTurnTextBlock` | TextBlock | 현재 턴 플레이어 |
| `TurnTimerTextBlock` | TextBlock | 남은 시간 |
| `ResultOverlay` | Overlay | 승리·무승부 결과 화면 |
| `ResultTextBlock` | TextBlock | 결과 문구 |

`PlayerController::BeginPlay()`에서는 `IsLocalController()`를 확인한 뒤에만 위젯을 생성합니다. 서버에 존재하는 원격 PlayerController나 Dedicated Server가 불필요한 UI를 생성하지 않도록 하기 위함입니다.

숫자 입력은 서버의 `ProcessPlayerInput()`에서 현재 턴과 남은 시간을 검사하므로 UI를 우회해도 거부됩니다. 같은 입력창에서 `/chat 내용`을 보내면 턴이나 시도 횟수와 관계없이 서버가 내용을 검증한 뒤 모든 클라이언트에 전달합니다.

## 프로젝트 구조

```text
Source/HW6
├─ Public
│  ├─ HW6GameMode.h
│  ├─ HW6GameState.h
│  ├─ HW6PlayerState.h
│  ├─ HW6PlayerController.h
│  └─ HW6MainWidget.h
├─ Private
│  ├─ HW6GameMode.cpp
│  ├─ HW6GameState.cpp
│  ├─ HW6PlayerState.cpp
│  ├─ HW6PlayerController.cpp
│  └─ HW6MainWidget.cpp
└─ HW6.Build.cs

Content
├─ NewMap.umap
└─ UI
   └─ WBP_MainWidget.uasset
```

## 실행 방법

1. Unreal Engine 5.5에서 `HW6.uproject`를 엽니다.
2. `NewMap`을 실행 맵으로 사용합니다.
3. PIE 설정에서 `Number of Players`를 2로 지정합니다.
4. `Net Mode`를 `Play As Listen Server`로 지정합니다.
5. 서버와 클라이언트 창을 실행합니다.
6. 현재 턴인 플레이어의 입력창에서 중복되지 않는 숫자 3개를 입력합니다.
7. 일반 채팅은 같은 입력창에 `/chat 안녕하세요`처럼 입력합니다. 버튼과 Enter 키를 모두 사용할 수 있습니다.

서버가 생성한 정답은 개발 테스트용으로 Output Log에서 확인할 수 있습니다.

```text
[Server] Secret Number: 386
```

## 테스트 체크리스트

### 입력 검증

| 입력 | 예상 결과 | 기회 소진 |
| --- | --- | --- |
| `12` | 3자리 입력 안내 | 없음 |
| `abc` | 숫자 입력 안내 | 없음 |
| `102` | 1~9 입력 안내 | 없음 |
| `112` | 중복 입력 안내 | 없음 |
| `386` | 정상 판정 | 1회 |

### 네트워크 및 게임 진행

- [ ] 서버와 클라이언트에 같은 판정 메시지가 표시되는가?
- [ ] `/chat 내용`이 모든 창에 표시되고 시도 횟수와 턴은 유지되는가?
- [ ] 각 창의 시도 횟수는 해당 로컬 플레이어의 값인가?
- [ ] 현재 턴과 남은 시간이 모든 창에서 동일한가?
- [ ] 현재 턴이 아닌 플레이어의 숫자 입력을 서버가 거부하는가?
- [ ] 콘솔 등으로 RPC를 강제 호출해도 서버가 거부하는가?
- [ ] 정상 입력 후 다음 플레이어로 턴이 넘어가는가?
- [ ] 잘못된 입력은 기회와 턴을 유지하는가?
- [ ] 시간 초과 시 기회가 1회 소진되고 턴이 넘어가는가?
- [ ] 3 Strike 시 승리 공지와 함께 입력 및 타이머가 정지하는가?
- [ ] 모든 플레이어가 기회를 소진하면 무승부가 되는가?
- [ ] 3초 후 정답·횟수·턴·타이머·결과 UI가 초기화되는가?

## 구현 중 해결한 문제

### 새 C++ 파일이 Rider에 보이지 않음

파일은 디스크에 존재하고 UnrealBuildTool 빌드에도 포함됐지만 기존 `.sln`의 파일 목록이 갱신되지 않아 Rider Solution에 표시되지 않았습니다. 새 클래스 파일을 외부에서 추가한 뒤 IDE가 감지하지 못하는 경우 프로젝트 파일을 다시 생성해 해결했습니다.

### `BindWidget` 타입 불일치

`GuessInputTextBox` 이름은 같았지만 Designer에서 `Editable Text`를 사용해 C++의 `UEditableTextBox`와 타입이 일치하지 않았습니다. Palette의 `Text Box`로 교체하고 이름과 `Is Variable`을 다시 확인했습니다.

### PIE 플레이어 이름 중복

같은 PC에서 실행한 PIE 창들이 같은 컴퓨터 이름을 사용했습니다. 서버 `PostLogin()`에서 `SetPlayerName()`을 호출해 접속 순서대로 `Player 1`, `Player 2`를 지정했습니다.

### 판정과 결과 공지가 반대 UI에 표시됨

일반 판정용 `MulticastBroadcastMessage()`와 라운드 결과용 `MulticastShowRoundResult()`의 호출 위치가 반대로 연결되어 있었습니다. `ProcessPlayerInput()`은 일반 판정, `EndRound()`는 최종 결과를 호출하도록 수정했습니다.

## 사용한 Unreal 자료구조와 기능

- `TArray<int32>`: 정답 숫자 및 PlayerArray 기반 순회
- `TWeakObjectPtr<AHW6PlayerState>`: 현재 턴 플레이어를 안전하게 참조
- `TObjectPtr`: Replication 및 UMG 객체 참조
- `TSubclassOf<UHW6MainWidget>`: 생성 가능한 Widget 클래스 제한
- `FTimerHandle`, `FTimerManager`: 턴 타이머와 라운드 리셋
- `FString`, `FText`: 네트워크 문자열과 UI 텍스트 처리
- `UPROPERTY(ReplicatedUsing)`: 상태 복제 및 변경 알림
- `UFUNCTION(Server/Client/NetMulticast)`: 실행 대상별 RPC
- Dynamic Delegate: `SubmitButton->OnClicked.AddDynamic()`
- Dynamic Delegate: `GuessInputTextBox->OnTextCommitted.AddDynamic()`

## 학습 내용

- 서버 권한은 Server RPC 선언만으로 완성되지 않으며, 서버 처리 함수에서 요청자·턴·시간·기회를 다시 검증해야 합니다.
- 비밀 데이터와 판정은 `GameMode`, 공유 상태는 `GameState`, 플레이어별 상태는 `PlayerState`가 담당하도록 책임을 나누었습니다.
- RPC는 사건 전달에, Replication은 지속 상태의 최신값 동기화에 적합합니다.
- `OnRep`는 클라이언트에서 호출되므로 Listen Server의 UI는 서버 Setter에서도 별도로 갱신해야 합니다.
- UI 비활성화는 사용자 경험을 위한 것이며, 서버 검사가 실제 입력 권한을 보장합니다.
- `BeginPlay`, `PostLogin`, `Logout`을 각각 게임 시작, 플레이어 접속, 퇴장 처리 시점에 사용했습니다.
- `BindWidget`은 이름뿐 아니라 실제 Widget 타입까지 일치해야 합니다.
- 서버 타이머와 복제 상태를 사용하면 모든 플레이어에게 같은 턴 규칙을 적용할 수 있습니다.
