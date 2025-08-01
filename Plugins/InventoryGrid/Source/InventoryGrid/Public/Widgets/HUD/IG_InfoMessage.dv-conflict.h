// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IG_InfoMessage.generated.h"

class UTextBlock;

UCLASS()
class INVENTORYGRID_API UIG_InfoMessage : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	void SetMessage(const FText& Message);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void MessageShow();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void MessageHide();

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Message;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MessageLifetime{3.f};

	FTimerHandle MessageTimer;
	bool bIsMessageActive{false};
};
