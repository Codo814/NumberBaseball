// NBNotificationText.cpp
#include "UI/NBNotificationText.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

TSharedRef<SWidget> UNBNotificationText::RebuildWidget()
{
	// 화면 상단 중앙에 공지를 만들고 마우스 입력은 채팅 위젯으로 통과시킵니다.
	SetVisibility(ESlateVisibility::HitTestInvisible);
	UCanvasPanel* CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = CanvasPanel;
	NotificationTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NotificationTextBlock->SetJustification(ETextJustify::Center);
	NotificationTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	FSlateFontInfo FontInfo = NotificationTextBlock->GetFont();
	FontInfo.Size = 24;
	NotificationTextBlock->SetFont(FontInfo);
	UCanvasPanelSlot* CanvasSlot = CanvasPanel->AddChildToCanvas(NotificationTextBlock);
	CanvasSlot->SetAnchors(FAnchors(0.5f, 0.f));
	CanvasSlot->SetAlignment(FVector2D(0.5f, 0.f));
	CanvasSlot->SetPosition(FVector2D(0.f, 30.f));
	CanvasSlot->SetAutoSize(true);
	return Super::RebuildWidget();
}

void UNBNotificationText::SetNotificationText(const FText& InNotificationText)
{
	// PlayerController에서 전달한 승리, 무승부 또는 시작 안내를 화면에 표시합니다.
	if (IsValid(NotificationTextBlock) == true)
	{
		NotificationTextBlock->SetText(InNotificationText);
	}
}