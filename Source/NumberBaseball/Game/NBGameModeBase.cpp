// NBGameModeBase.cpp
#include "Game/NBGameModeBase.h"
#include "Game/NBGameStateBase.h"
#include "Player/NBPlayerState.h"
#include "Player/NBPlayerController.h"
#include "TimerManager.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPlayerController) == false)
	{
		return;
	}
	AllPlayerControllers.AddUnique(NBPlayerController);
	ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS) == true)
	{
		// 퇴장 후 새 플레이어가 들어와도 이름이 중복되지 않도록 번호 증가
		NBPS->PlayerNameString = TEXT("Player") + FString::FromInt(NextPlayerNumber++);
		ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase) == true)
		{
			NBGameStateBase->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}
	NBPlayerController->SetNotificationText(bIsRoundOver ? RoundResultText : FText::FromString(TEXT("숫자 3자리를 입력하세요. 일반 채팅: /chat 메시지")));
}

void ANBGameModeBase::Logout(AController* Exiting)
{
	// 퇴장한 플레이어를 제거하고 남은 플레이어의 무승부 조건을 다시 확인
	AllPlayerControllers.Remove(Cast<ANBPlayerController>(Exiting));
	Super::Logout(Exiting);
	if (AllPlayerControllers.IsEmpty())
	{
		ResetGame();
	}
	else
	{
		JudgeGame(nullptr, 0);
	}
}

FString ANBGameModeBase::GenerateSecretNumber()
{
	// 선택한 숫자를 후보에서 제거하여 1~9 중 중복 없는 세 자리를 생성
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}
	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	return Result;
}

FString ANBGameModeBase::GetGuessValidationMessage(const FString& InNumberString)
{
	// 입력 전체의 길이, 숫자 범위, 중복 여부를 순서대로 검사
	if (InNumberString.Len() != 3)
	{
		return TEXT("3자리 숫자를 입력해주세요.");
	}
	TSet<TCHAR> UniqueDigits;
	for (TCHAR C : InNumberString)
	{
		if (C < TEXT('1') || C > TEXT('9'))
		{
			return TEXT("1~9 사이의 숫자만 입력해주세요.");
		}
		UniqueDigits.Add(C);
	}
	if (UniqueDigits.Num() != 3)
	{
		return TEXT("중복되지 않은 숫자를 입력해주세요.");
	}
	return FString();
}

bool ANBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	// 오류 안내가 비어 있으면 규칙에 맞는 추측
	return GetGuessValidationMessage(InNumberString).IsEmpty();
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	if (IsGuessNumberString(InSecretNumberString) == false || IsGuessNumberString(InGuessNumberString) == false)
	{
		return TEXT("잘못된 숫자입니다.");
	}
	// 같은 자리의 숫자는 Strike, 다른 자리에 존재하는 숫자는 Ball로 계산
	int32 StrikeCount = 0, BallCount = 0;
	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else if (InSecretNumberString.Contains(FString::Printf(TEXT("%c"), InGuessNumberString[i])))
		{
			BallCount++;
		}
	}
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}
	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	// GameMode는 서버에만 존재하므로 정답도 서버에서 생성
	ResetGame();
}

void ANBGameModeBase::PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (IsValid(InChattingPlayerController) == false)
	{
		return;
	}
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS) == false)
	{
		return;
	}
	// /chat 명령은 기회를 소진하지 않는 일반 채팅으로 모든 플레이어에게 전달
	if (InChatMessageString.StartsWith(TEXT("/chat ")))
	{
		FString Message = NBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString.RightChop(6);
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			if (IsValid(NBPlayerController) == true)
			{
				NBPlayerController->ClientRPCPrintChatMessageString(Message);
			}
		}
		return;
	}
	// 결과 공지 중에는 추측을 거절하여 다음 라운드의 데이터가 섞이지 않게 함
	if (bIsRoundOver == true)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("게임이 종료되었습니다. 잠시 후 다시 시작합니다."));
		return;
	}
	FString ValidationMessage = GetGuessValidationMessage(InChatMessageString);
	if (ValidationMessage.IsEmpty() == false)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(ValidationMessage);
		return;
	}
	// 서버에서 최대 시도 횟수를 검사하여 네 번째 추측을 차단
	if (NBPS->CurrentGuessCount >= NBPS->MaxGuessCount)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("기회를 모두 사용했습니다. 다른 플레이어를 기다려주세요."));
		return;
	}
	IncreaseGuessCount(InChattingPlayerController);
	FString JudgeResultString = JudgeResult(SecretNumberString, InChatMessageString);
	// 횟수를 증가시킨 뒤 메시지를 만들어 첫 입력부터 [1/3]으로 표시
	FString CombinedMessageString = NBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;
	for (const auto& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == true)
		{
			NBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
		}
	}
	// 모든 플레이어에게 추측 결과를 보낸 후 게임 판정을 한 번만 실행
	int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
	JudgeGame(InChattingPlayerController, StrikeCount);
}

void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChattingPlayerController)
{
	if (IsValid(InChattingPlayerController) == false)
	{
		return;
	}
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	// 유효한 추측에 대해서만 최대 횟수 안에서 시도 횟수를 증가
	if (IsValid(NBPS) == true && NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
	{
		NBPS->CurrentGuessCount++;
	}
}

void ANBGameModeBase::ResetGame()
{
	// 타이머, 종료 상태, 정답과 모든 플레이어의 시도 횟수를 한 번에 초기화
	GetWorldTimerManager().ClearTimer(ResetGameTimerHandle);
	bIsRoundOver = false;
	RoundResultText = FText::GetEmpty();
	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("NumberBaseball secret: %s"), *SecretNumberString);
	for (const auto& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == false)
		{
			continue;
		}
		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->CurrentGuessCount = 0;
		}
		NBPlayerController->SetNotificationText(FText::FromString(TEXT("새 게임 시작! 숫자 3자리를 입력하세요.")));
	}
}

void ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (bIsRoundOver == true || AllPlayerControllers.IsEmpty())
	{
		return;
	}
	FString ResultMessage;
	ANBPlayerState* NBPS = IsValid(InChattingPlayerController) ? InChattingPlayerController->GetPlayerState<ANBPlayerState>() : nullptr;
	// 세 번 이내에 정답을 맞힌 플레이어를 즉시 승자로 결정
	if (InStrikeCount == 3 && IsValid(NBPS) == true && NBPS->CurrentGuessCount <= NBPS->MaxGuessCount)
	{
		ResultMessage = NBPS->PlayerNameString + TEXT(" 승리!");
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			ANBPlayerState* PlayerState = IsValid(NBPlayerController) ? NBPlayerController->GetPlayerState<ANBPlayerState>() : nullptr;
			if (IsValid(PlayerState) == false || PlayerState->CurrentGuessCount < PlayerState->MaxGuessCount)
			{
				bIsDraw = false;
				break;
			}
		}
		if (bIsDraw == true)
		{
			ResultMessage = TEXT("무승부! 모든 플레이어가 기회를 사용했습니다.");
		}
	}
	if (ResultMessage.IsEmpty())
	{
		return;
	}
	bIsRoundOver = true;
	RoundResultText = FText::FromString(ResultMessage);
	for (const auto& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == true)
		{
			NBPlayerController->SetNotificationText(RoundResultText);
		}
	}
	// 공지를 읽을 수 있도록 3초 뒤 새 라운드를 한 번만 시작
	GetWorldTimerManager().SetTimer(ResetGameTimerHandle, this, &ThisClass::ResetGame, 3.f, false);
}