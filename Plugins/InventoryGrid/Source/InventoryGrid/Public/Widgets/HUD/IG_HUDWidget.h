// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IG_HUDWidget.generated.h"

class UIG_InfoMessage;

UCLASS()
class INVENTORYGRID_API UIG_HUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void ShowPickupMessage(const FString& Message);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void HidePickupMessage();

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UIG_InfoMessage> InfoMessage;

	UFUNCTION()
	void OnNoRoom();
};
