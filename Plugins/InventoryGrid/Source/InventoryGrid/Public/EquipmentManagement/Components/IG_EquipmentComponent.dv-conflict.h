// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IG_EquipmentComponent.generated.h"

struct FGameplayTag;
struct FIG_ItemManifest;
struct FIG_EquipmentFragment;
class AIG_EquipActor;
class UIG_InventoryItem;
class UIG_InventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORYGRID_API UIG_EquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	void InitializeOwner(APlayerController* PlayerController);
	void SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh);
	void SetIsProxy(const bool bProxy) { bIsProxy = bProxy; };
	
protected:
	
	virtual void BeginPlay() override;

private:

	void InitPlayerController();
	void InitInventoryComponent();
	AIG_EquipActor* SpawnEquippedActor(FIG_EquipmentFragment* EquipmentFragment, const FIG_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh) const;
	AIG_EquipActor* FindEquippedActorByTag(const FGameplayTag& EquipmentTypeTag);
	void RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag);

	UPROPERTY()
	TArray<TObjectPtr<AIG_EquipActor>> EquippedActors;

	bool bIsProxy{false};
	
	TWeakObjectPtr<UIG_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

	UFUNCTION()
	void OnItemEquipped(UIG_InventoryItem* EquippedItem);
	UFUNCTION()
	void OnItemUnequipped(UIG_InventoryItem* UnequippedItem);

	UFUNCTION()
	void OnPossessedPawnChange(APawn* OldPawn, APawn* NewPawn);
};
