// NBNotificationText.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NBNotificationText.generated.h"

class UTextBlock;

UCLASS()
class NUMBERBASEBALL_API UNBNotificationText : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	void SetNotificationText(const FText& InNotificationText);

protected:
	UPROPERTY()
	TObjectPtr<UTextBlock> NotificationTextBlock;
};