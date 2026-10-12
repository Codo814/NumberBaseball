// NBPlayerController.cpp

#include "Player/NBPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Game/NBGameModeBase.h"
#include "NBPlayerState.h"
#include "UI/NBChatInput.h"
#include "UI/NBNotificationText.h"
#include "NumberBaseball.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"


ANBPlayerController::ANBPlayerController()
{
	bReplicates = true;
}
void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UNBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	// 기본 공지 위젯을 C++로 생성하여 Blueprint 텍스트 바인딩 없이 결과를 표시
	TSubclassOf<UUserWidget> WidgetClass = UNBNotificationText::StaticClass();
	if (NotificationTextWidgetClass != nullptr && NotificationTextWidgetClass->IsChildOf(UNBNotificationText::StaticClass()))
	{
		WidgetClass = NotificationTextWidgetClass;
	}
	NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, WidgetClass);
	if (IsValid(NotificationTextWidgetInstance) == true)
	{
		NotificationTextWidgetInstance->AddToViewport(10);
		OnRepNotificationText();
	}
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	// 이름과 횟수를 붙이지 않고 원본 입력을 서버로 보내 전체 문자열을 검사
	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}
void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	//UKismetSystemLibrary::PrintString(this, chatMessageString, true, true, FLinearColor::Red, 5.0f);

	FString NetModeString = NumberBaseballFunctionLibrary::GetNetModeString(this);
	FString CombinedMessageString = FString::Printf(TEXT("%s, %s"), *NetModeString, *InChatMessageString);
	NumberBaseballFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);

}


void ANBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ANBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	// 판정과 기회 관리는 서버의 GameMode에 맡김
	ANBGameModeBase* NBGM = Cast<ANBGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(NBGM) == true)
	{
		NBGM->PrintChatMessageString(this, InChatMessageString);
	}
}

void ANBPlayerController::SetNotificationText(const FText& InNotificationText)
{
	// 서버에서 공지 값을 변경하고 리슨 서버 화면도 즉시 갱신
	if (HasAuthority() == true)
	{
		NotificationText = InNotificationText;
		OnRepNotificationText();
		ForceNetUpdate();
	}
}


inline void ANBPlayerController::OnRepNotificationText()
{
	// 복제된 공지를 소유 클라이언트의 위젯에 적용
	UNBNotificationText* NotificationWidget = Cast<UNBNotificationText>(NotificationTextWidgetInstance);
	if (IsValid(NotificationWidget) == true)
	{
		NotificationWidget->SetNotificationText(NotificationText);
	}
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, NotificationText);
}
