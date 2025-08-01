// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/IG_InfoMessage.h"

#include "Components/TextBlock.h"

void UIG_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Text_Message->SetText(FText::GetEmpty());
	MessageHide();
}

void UIG_InfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);

	if (!bIsMessageActive) MessageShow();
	bIsMessageActive = true;

	GetWorld()->GetTimerManager().SetTimer(MessageTimer, [this]()
		{
			MessageHide();
			bIsMessageActive = false;
		},
		MessageLifetime,
		false
	);
}
