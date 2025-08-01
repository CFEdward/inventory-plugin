// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/IG_InventoryBase.h"
#include "IG_InventorySpatial.generated.h"

class UIG_EquippedSlottedItem;
struct FGameplayTag;
class UIG_EquippedGridSlot;
class UIG_ItemDescription;
class UCanvasPanel;
class UButton;
class UWidgetSwitcher;
class UIG_InventoryGrid;

UCLASS()
class INVENTORYGRID_API UIG_InventorySpatial : public UIG_InventoryBase
{
	GENERATED_BODY()

public:

	virtual FIG_SlotAvailabilityResult HasRoomForItem(UIG_ItemComponent* ItemComponent) const override;

	virtual void OnItemHovered(UIG_InventoryItem* Item) override;
	virtual void OnItemUnhovered() override;
	virtual bool HasHoverItem() const override;
	virtual UIG_HoverItem* GetHoverItem() const override;
	virtual float GetTileSize() const override;

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
private:

	void SetActiveGrid(UIG_InventoryGrid* Grid, UButton* Button);
	void DisableButton(UButton* Button) const;
	void SetItemDescriptionSizeAndPosition(UIG_ItemDescription* Description, UCanvasPanel* Canvas) const;
	bool CanEquipHoverItem(const UIG_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;
	UIG_EquippedGridSlot* FindSlotWithEquippedItem(UIG_InventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UIG_EquippedGridSlot* EquippedGridSlot);
	void RemoveEquippedSlottedItem(UIG_EquippedSlottedItem* EquippedSlottedItem);
	void MakeEquippedSlottedItem(const UIG_EquippedSlottedItem* EquippedSlottedItem, UIG_EquippedGridSlot* EquippedGridSlot, UIG_InventoryItem* ItemToEquip);
	void BroadcastSlotClickedDelegates(UIG_InventoryItem* ItemToEquip, UIG_InventoryItem* ItemToUnequip) const;

	UFUNCTION()
	void EquippedSlottedItemClicked(UIG_EquippedSlottedItem* EquippedSlottedItem);
	
	UPROPERTY()
	TArray<TObjectPtr<UIG_EquippedGridSlot>> EquippedGridSlots;
	UFUNCTION()
	void EquippedGridSlotClicked(UIG_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UIG_InventoryGrid> Grid_Equippables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Equippables;
	UFUNCTION()
	void ShowEquippables();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UIG_InventoryGrid> Grid_Consumables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consumables;
	UFUNCTION()
	void ShowConsumables();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UIG_InventoryGrid> Grid_Craftables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Craftables;
	UFUNCTION()
	void ShowCraftables();

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UIG_ItemDescription> ItemDescriptionClass;
	UPROPERTY()
	TObjectPtr<UIG_ItemDescription> ItemDescription;
	UIG_ItemDescription* GetItemDescription();
	FTimerHandle DescriptionTimer;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionTimerDelay{.5f};
	
	TWeakObjectPtr<UIG_InventoryGrid> ActiveGrid;
};
