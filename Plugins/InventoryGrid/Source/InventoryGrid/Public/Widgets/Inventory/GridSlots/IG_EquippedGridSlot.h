// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IG_GridSlot.h"
#include "IG_EquippedGridSlot.generated.h"

class UOverlay;
class UIG_EquippedSlottedItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked, UIG_EquippedGridSlot*, GridSlot,
                                             const FGameplayTag&, EquipmentTypeTag);

UCLASS()
class INVENTORYGRID_API UIG_EquippedGridSlot : public UIG_GridSlot
{
	GENERATED_BODY()

public:

	UIG_EquippedSlottedItem* OnItemEquipped(UIG_InventoryItem* Item, const FGameplayTag& EquipmentTag, const float TileSize);

	FEquippedGridSlotClicked EquippedGridSlotClicked;

	void SetEquippedSlottedItem(UIG_EquippedSlottedItem* Item) { EquippedSlottedItem = Item; }

protected:

	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GreyedOutIcon;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UIG_EquippedSlottedItem> EquippedSlottedItemClass;
	UPROPERTY()
	TObjectPtr<UIG_EquippedSlottedItem> EquippedSlottedItem;
};
