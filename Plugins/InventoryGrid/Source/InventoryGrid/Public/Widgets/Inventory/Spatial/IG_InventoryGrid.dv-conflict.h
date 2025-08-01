// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/IG_GridTypes.h"
#include "IG_InventoryGrid.generated.h"

class UIG_ItemPopUp;
enum class EIG_GridSlotState : uint8;
class UIG_HoverItem;
struct FGameplayTag;
struct FIG_ImageFragment;
struct FIG_GridFragment;
class UIG_SlottedItem;
struct FIG_ItemManifest;
class UIG_ItemComponent;
class UIG_InventoryComponent;
class UCanvasPanel;
class UIG_GridSlot;

UCLASS()
class INVENTORYGRID_API UIG_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	
	FIG_SlotAvailabilityResult HasRoomForItem(const UIG_ItemComponent* ItemComponent);
	UFUNCTION()
	void AddItem(UIG_InventoryItem* Item);
	void DropItem();

	void ShowCursor();
	void HideCursor();

	bool HasHoverItem() const;
	void AssignHoverItem(UIG_InventoryItem* InventoryItem);
	void ClearHoverItem();

	void OnHide();

	void SetOwningCanvas(UCanvasPanel* OwningCanvas);
	EIG_ItemCategory GetItemCategory() const { return ItemCategory; }
	UIG_HoverItem* GetHoverItem() const { return HoverItem; }
	float GetTileSize() const { return TileSize; }

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UIG_GridSlot> GridSlotClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Rows;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Columns;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	float TileSize;

private:

	void ConstructGrid();

	FIG_SlotAvailabilityResult HasRoomForItem(const UIG_InventoryItem* Item, const int32 StackAmountOverride = -1);
	FIG_SlotAvailabilityResult HasRoomForItem(const FIG_ItemManifest& Manifest, const int32 StackAmountOverride = -1);
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const;
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;
	FIntPoint GetItemDimensions(const FIG_ItemManifest& Manifest) const;
	bool HasRoomAtIndex(
		const UIG_GridSlot* GridSlot,
		const FIntPoint& Dimensions,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize
	);
	bool CheckSlotConstraints(
		const UIG_GridSlot* GridSlot,
		const UIG_GridSlot* SubGridSlot,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize
	) const;
	int32 DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UIG_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UIG_GridSlot* GridSlot) const;
	bool HasValidItem(const UIG_GridSlot* GridSlot) const;
	bool IsUpperLeftSlot(const UIG_GridSlot* GridSlot, const UIG_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UIG_InventoryItem* SubItem, const FGameplayTag& ItemType) const;
	bool MatchesCategory(const UIG_InventoryItem* Item) const;
	void AddItemToIndices(const FIG_SlotAvailabilityResult& Result, UIG_InventoryItem* NewItem);
	void AddItemAtIndex(UIG_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	void UpdateGridSlots(UIG_InventoryItem* NewItem, const int32 Index, const bool bStackableItem, const int32 StackAmount);
	UIG_SlottedItem* CreateSlottedItem(
		UIG_InventoryItem* Item,
		const bool bStackable,
		const int32 StackAmount,
		const FIG_GridFragment* GridFragment,
		const FIG_ImageFragment* ImageFragment,
		const int32 Index
	) const;
	void AddSlottedItemToCanvas(const int32 Index, const FIG_GridFragment* GridFragment, UIG_SlottedItem* SlottedItem) const;
	FVector2D GetDrawSize(const FIG_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UIG_SlottedItem* SlottedItem, const FIG_GridFragment* GridFragment, const FIG_ImageFragment* ImageFragment) const;

	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	
	UFUNCTION()
	void AddStacks(const FIG_SlotAvailabilityResult& Result);

	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);
	void CreateItemPopUp(const int32 GridIndex);
	void PickUp(UIG_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	void AssignHoverItem(UIG_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex);
	void RemoveItemFromGrid(const UIG_InventoryItem* InventoryItem, const int32 GridIndex);

	bool CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	EIG_TileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	void OnTileParametersUpdated(const FIG_TileParameters& Parameters);
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EIG_TileQuadrant Quadrant) const;
	FIG_SpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);
	void PutDownOnIndex(const int32 Index);

	void HighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, const EIG_GridSlotState GridSlotState);
	void PutHoverItemBack();
	UFUNCTION()
	void OnInventoryMenuToggled(const bool bOpen);

	UFUNCTION()
	void OnGridSlotClicked(const int32 GridIndex, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnGridSlotHovered(const int32 GridIndex, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnGridSlotUnhovered(const int32 GridIndex, const FPointerEvent& MouseEvent);

	bool IsSameStackable(const UIG_InventoryItem* ClickedInventoryItem) const;
	void SwapWithHoverItem(UIG_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	bool ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;
	void SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldConsumeHoverItemStacks(const int32 HoveredStackCount, const int32 RoomInClickedSlot) const;
	void ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const;
	void FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index);

	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UIG_ItemPopUp> ItemPopUpClass;
	UPROPERTY()
	TObjectPtr<UIG_ItemPopUp> ItemPopUp;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D ItemPopUpOffset;

	UFUNCTION()
	void OnPopUpMenuSplit(const int32 SplitAmount, const int32 Index);
	UFUNCTION()
	void OnPopUpMenuDrop(const int32 Index);
	UFUNCTION()
	void OnPopUpMenuConsume(const int32 Index);
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;
	UUserWidget* GetVisibleCursorWidget();
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;
	UUserWidget* GetHiddenCursorWidget();
	
	TWeakObjectPtr<UIG_InventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	EIG_ItemCategory ItemCategory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UIG_SlottedItem> SlottedItemClass;
	UPROPERTY()
	TMap<int32, TObjectPtr<UIG_SlottedItem>> SlottedItems;
	UPROPERTY()
	TArray<TObjectPtr<UIG_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UIG_HoverItem> HoverItemClass;
	UPROPERTY()
	TObjectPtr<UIG_HoverItem> HoverItem;

	FIG_TileParameters TileParameters;
	FIG_TileParameters LastTileParameters;
	/** Index where an item would be placed if we click on the grid at a valid location */
	int32 ItemDropIndex{INDEX_NONE};
	FIG_SpaceQueryResult CurrentQueryResult;
	int32 LastHighlightedIndex{INDEX_NONE};
	FIntPoint LastHighlightedDimensions;

	bool bMouseWithinCanvas;
	bool bLastMouseWithinCanvas;
};
