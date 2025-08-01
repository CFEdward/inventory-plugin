// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IG_PlayerController.generated.h"

class UIG_InventoryComponent;
class UIG_HUDWidget;
class UInputAction;
class UInputMappingContext;

UCLASS()
class INVENTORYGRID_API AIG_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AIG_PlayerController();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ToggleInventory();
	
protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	
	TWeakObjectPtr<UIG_InventoryComponent> InventoryComponent;
	
	void TraceForItem();
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	double TraceLength;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> ItemTraceChannel;
	TWeakObjectPtr<AActor> ThisActor;
	TWeakObjectPtr<AActor> LastActor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<TObjectPtr<UInputMappingContext>> DefaultIMCs;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractAction;
	void PrimaryInteract();
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	void CreateHUDWidget();
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UIG_HUDWidget> HUDWidgetClass;
	UPROPERTY()
	TObjectPtr<UIG_HUDWidget> HUDWidget;
};
