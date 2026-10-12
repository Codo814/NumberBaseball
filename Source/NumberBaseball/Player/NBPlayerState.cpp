//NBPlayerState.cpp

#include "NBPlayerState.h"
#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState()
	:PlayerNameString(TEXT("None")),
	CurrentGuessCount(0),
	MaxGuessCount(3)
{
	bReplicates = true;
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ANBPlayerState::GetPlayerInfoString()
{
	// 서버에서 갱신된 현재 횟수와 최대 횟수를 출력용 문자열로 반환
	FString PlayerInfoString = PlayerNameString + TEXT("[") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT("]");
	return PlayerInfoString;
}
