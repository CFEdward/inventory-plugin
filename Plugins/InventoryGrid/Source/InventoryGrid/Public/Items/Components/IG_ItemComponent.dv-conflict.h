// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/IG_ItemManifest.h"
#include "IG_ItemComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORYGRID_API UIG_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UIG_ItemComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void PickedUp();

	void InitItemManifest(FIG_ItemManifest CopyOfManifest);
	FIG_ItemManifest GetItemManifest() const { return ItemManifest; }
	FString GetPickupMessage() const { return PickupMessage; }
	
protected:

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnPickedUp();	

private:

	UPROPERTY(EditAnywhere, Replicated, Category = "Inventory")
	FIG_ItemManifest ItemManifest;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;
};
