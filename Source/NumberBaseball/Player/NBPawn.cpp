// NBPawn.cpp

#include "NBPawn.h"
#include "NumberBaseball.h"

void ANBPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = NumberBaseballFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBPawn::BeginPlay() %s [%s]"), *NumberBaseballFunctionLibrary::GetNetModeString(this), *NetRoleString);
	NumberBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ANBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = NumberBaseballFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBPawn::PossessedBy() %s [%s]"), *NumberBaseballFunctionLibrary::GetNetModeString(this), *NetRoleString);
	NumberBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}