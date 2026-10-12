// NBGameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;

UCLASS()
class NUMBERBASEBALL_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void BeginPlay() override;
	FString GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString GetGuessValidationMessage(const FString& InNumberString);
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	void PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	void IncreaseGuessCount(ANBPlayerController* InChattingPlayerController);
	void ResetGame();
	void JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;
	// 접속 중인 플레이어만 결과 판정과 공지 대상에 포함
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;
	bool bIsRoundOver = false;
	int32 NextPlayerNumber = 1;
	FText RoundResultText;
	FTimerHandle ResetGameTimerHandle;
};