// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IG_GridSlot.generated.h"

class UIG_ItemPopUp;
class UIG_InventoryItem;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

UENUM(BlueprintType)
enum class EIG_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GreyedOut
};

UCLASS()
class INVENTORYGRID_API UIG_GridSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	
	int32 GetTileIndex() const { return TileIndex; }
	void SetTileIndex(const int32 Index) { TileIndex = Index; }
	EIG_GridSlotState GetGridSlotState() const { return GridSlotState; }
	TWeakObjectPtr<UIG_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UIG_InventoryItem* Item);
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(const int32 Count) { StackCount = Count; }
	int32 GetIndex() const { return TileIndex; }
	void SetIndex(const int32 Index) { TileIndex = Index; }
	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(const int32 Index) { UpperLeftIndex = Index; }
	bool IsAvailable() const { return bAvailable; }
	void SetAvailable(const bool bIsAvailable) { bAvailable = bIsAvailable; }
	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGreyedOutTexture();
	void SetItemPopUp(UIG_ItemPopUp* PopUp);
	UIG_ItemPopUp* GetItemPopUp() const;

	FGridSlotEvent GridSlotClicked;
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;

protected:

	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
private:

	TWeakObjectPtr<UIG_ItemPopUp> ItemPopUp;
	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget* Menu);
	
	int32 TileIndex{INDEX_NONE};
	int32 UpperLeftIndex{INDEX_NONE};
	int32 StackCount{0};
	bool bAvailable{true};
	TWeakObjectPtr<UIG_InventoryItem> InventoryItem;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GreyedOut;
	
	EIG_GridSlotState GridSlotState;
};
