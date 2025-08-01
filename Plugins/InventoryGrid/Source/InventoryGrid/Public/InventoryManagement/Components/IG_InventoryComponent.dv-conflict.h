// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/IG_FastArray.h"
#include "IG_InventoryComponent.generated.h"

class UIG_ItemComponent;
class UIG_InventoryBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UIG_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChange, const FIG_SlotAvailabilityResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChanged, UIG_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryMenuToggled, bool, bOpen);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORYGRID_API UIG_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UIG_InventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void TryAddItem(UIG_ItemComponent* ItemComponent);
	UFUNCTION(Server, Reliable)
	void Server_AddNewItem(UIG_ItemComponent* ItemComponent, int32 StackCount);
	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UIG_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);
	UFUNCTION(Server, Reliable)
	void Server_DropItem(UIG_InventoryItem* Item, const int32 StackCount);
	UFUNCTION(Server, Reliable)
	void Server_ConsumeItem(UIG_InventoryItem* Item);

	UFUNCTION(Server, Reliable)
	void Server_EquipSlotClicked(UIG_InventoryItem* ItemToEquip, UIG_InventoryItem* ItemToUnequip);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipSlotClicked(UIG_InventoryItem* ItemToEquip, UIG_InventoryItem* ItemToUnequip);

	void ToggleInventoryMenu();

	void AddRepSubObj(UObject* SubObj);

	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved;
	FNoRoomInInventory NoRoomInInventory;
	FStackChange OnStackChange;
	FItemEquipStatusChanged OnItemEquipped;
	FItemEquipStatusChanged OnItemUnequipped;
	FInventoryMenuToggled OnInventoryMenuToggled;

	UIG_InventoryBase* GetInventoryMenu() const { return InventoryMenu; }
	bool IsMenuOpen() const { return bInventoryMenuOpen; }

protected:
	
	virtual void BeginPlay() override;

private:

	void SpawnDroppedItem(UIG_InventoryItem* Item, const int32 StackCount) const;
	
	UPROPERTY(Replicated)
	FIG_InventoryFastArray InventoryList;
	
	void ConstructInventory();
	TWeakObjectPtr<APlayerController> OwningController;
	UPROPERTY()
	TObjectPtr<UIG_InventoryBase> InventoryMenu;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UIG_InventoryBase> InventoryMenuClass;

	bool bInventoryMenuOpen{false};
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMin{-85.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMax{85.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMin{10.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMax{50.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float RelativeSpawnElevation{70.f};
};
