// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IG_SlottedItem.h"
#include "IG_EquippedSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked, UIG_EquippedSlottedItem*, SlottedItem);

UCLASS()
class INVENTORYGRID_API UIG_EquippedSlottedItem : public UIG_SlottedItem
{
	GENERATED_BODY()

public:

	FEquippedSlottedItemClicked OnEquippedSlottedItemClicked;

	void SetEquipmentTypeTag(const FGameplayTag& Tag) { EquipmentTypeTag = Tag; }
	FGameplayTag GetEquipmentTypeTag() const { return EquipmentTypeTag; }

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	
private:

	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
};
