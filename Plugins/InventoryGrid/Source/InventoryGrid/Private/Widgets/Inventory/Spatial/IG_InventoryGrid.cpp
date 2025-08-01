// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/IG_InventoryGrid.h"

#include "InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "InventoryManagement/Utils/IG_InventoryStatics.h"
#include "Items/IG_InventoryItem.h"
#include "Items/Components/IG_ItemComponent.h"
#include "Items/Fragments/IG_FragmentTags.h"
#include "Items/Fragments/IG_ItemFragment.h"
#include "Widgets/Inventory/GridSlots/IG_GridSlot.h"
#include "Widgets/Inventory/HoverItem/IG_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/IG_SlottedItem.h"
#include "Widgets/ItemPopUp/IG_ItemPopUp.h"
#include "Widgets/Utils/IG_WidgetUtils.h"

void UIG_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UIG_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::AddStacks);
	InventoryComponent->OnInventoryMenuToggled.AddDynamic(this, &ThisClass::OnInventoryMenuToggled);
}

void UIG_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			UIG_GridSlot* GridSlot = CreateWidget<UIG_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChildToCanvas(GridSlot);

			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UIG_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);

			GridSlots.Add(GridSlot);
			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);
		}
	}
}

void UIG_InventoryGrid::OnGridSlotClicked(const int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	if (!IsInGridBounds(ItemDropIndex, HoverItem->GetGridDimensions())) return;
	
	const auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UIG_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UIG_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;

	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());

	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	ShowCursor();
}

void UIG_InventoryGrid::OnHide()
{
	PutHoverItemBack();
}

void UIG_InventoryGrid::OnGridSlotHovered(const int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UIG_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UIG_InventoryGrid::OnGridSlotUnhovered(const int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UIG_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

bool UIG_InventoryGrid::IsSameStackable(const UIG_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable;
}

void UIG_InventoryGrid::SwapWithHoverItem(UIG_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;

	UIG_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = ClickedInventoryItem->IsStackable();

	// Keep the same previous grid index
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UIG_InventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UIG_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	UIG_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoveredStackCount);
	const UIG_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UIG_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredStackCount, const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoveredStackCount;
}

void UIG_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	const int32 AmountToTransfer = HoveredStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;

	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();

	const FIG_GridFragment* GridFragment = GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FIG_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1);
	HighlightSlots(Index, Dimensions);
}

bool UIG_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UIG_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UIG_GridSlot* GridSlot = GridSlots[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;
	GridSlot->SetStackCount(NewStackCount);
	
	const UIG_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);

	HoverItem->UpdateStackCount(Remainder);
}

void UIG_InventoryGrid::OnPopUpMenuSplit(const int32 SplitAmount, const int32 Index)
{
	UIG_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem) || !RightClickedItem->IsStackable()) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UIG_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;
	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UIG_InventoryGrid::OnPopUpMenuDrop(const int32 Index)
{
	UIG_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;

	PickUp(RightClickedItem, Index);
	DropItem();
}

void UIG_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem) || !IsValid(HoverItem->GetInventoryItem())) return;

	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());

	ClearHoverItem();
	ShowCursor();
}

void UIG_InventoryGrid::OnPopUpMenuConsume(const int32 Index)
{
	UIG_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UIG_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 NewStackCount = UpperLeftGridSlot->GetStackCount() - 1;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	InventoryComponent->Server_ConsumeItem(RightClickedItem);

	if (NewStackCount <= 0)
	{
		RemoveItemFromGrid(RightClickedItem, Index);
	}
}

void UIG_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UIG_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

bool UIG_InventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

UUserWidget* UIG_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;

	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}

	return VisibleCursorWidget;
}

UUserWidget* UIG_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;

	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	
	return HiddenCursorWidget;
}

void UIG_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D CanvasPosition = UIG_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	
	if (CursorExitedCanvas(CanvasPosition, UIG_WidgetUtils::GetWidgetSize(CanvasPanel), MousePosition))
	{
		return;
	}
	
	UpdateTileParameters(CanvasPosition, MousePosition);
}

bool UIG_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UIG_WidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, Location);
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}

	return false;
}

void UIG_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithinCanvas) return;
	
	// Calculate the tile quadrant, tile index and coordinates
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	LastTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = UIG_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);
	
	OnTileParametersUpdated(TileParameters);
}

FIntPoint UIG_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
}

EIG_TileQuadrant UIG_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// Calculate relative position within the current tile
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	// Determine which quadrant the mouse is in
	const bool bIsTop = TileLocalY < TileSize / 2.f;	// Top if Y is in the upper half
	const bool bIsLeft = TileLocalX < TileSize / 2.f;	// Left if X is in the left half

	EIG_TileQuadrant HoveredTileQuadrant{EIG_TileQuadrant::None};
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EIG_TileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = EIG_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EIG_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EIG_TileQuadrant::BottomRight;

	return HoveredTileQuadrant;
}

void UIG_InventoryGrid::OnTileParametersUpdated(const FIG_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;

	// Get Hover Item's dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	// Calculate the starting coordinate for highlighting
	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = UIG_WidgetUtils::GetIndexFromPosition(StartingCoordinate, Columns);
	
	// Check hover position
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		const FIG_GridFragment* GridFragment = GetFragment<FIG_GridFragment>(CurrentQueryResult.ValidItem.Get(), FragmentTags::Grid);
		if (!GridFragment) return;

		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EIG_GridSlotState::GreyedOut);
	}
}

FIntPoint UIG_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EIG_TileQuadrant Quadrant) const
{
	const uint8 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const uint8 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EIG_TileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EIG_TileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EIG_TileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case EIG_TileQuadrant::BottomRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant"));
		return FIntPoint(INDEX_NONE);
	}

	return StartingCoord;
}

FIG_SpaceQueryResult UIG_InventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FIG_SpaceQueryResult Result;
	
	// In the grid bounds?
	if (!IsInGridBounds(UIG_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;
	Result.bHasSpace = true;
	
	// If more than one of the indices is occupied with the same item,
	// See if they all have the same upper-left index
	TSet<int32> OccupiedUpperLeftIndices;
	UIG_InventoryStatics::ForEach2D(GridSlots, UIG_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns, [&](const UIG_GridSlot* GridSlot)
		{
			if (GridSlot->GetInventoryItem().IsValid())
			{
				OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
				Result.bHasSpace = false;
			}
		}
	);
	// If so, is there only one item in the way? (can we swap?)
	if (OccupiedUpperLeftIndices.Num() == 1)	// Single item at position - it's valid for swapping/combining
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}
	
	return Result;
}

void UIG_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;
	
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UIG_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UIG_GridSlot* GridSlot)
		{
			GridSlot->SetOccupiedTexture();
		}
	);
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UIG_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UIG_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UIG_GridSlot* GridSlot)
		{
			if (GridSlot->IsAvailable())
			{
				GridSlot->SetUnoccupiedTexture();
			}
			else
			{
				GridSlot->SetOccupiedTexture();
			}
		}
	);
}

void UIG_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, const EIG_GridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UIG_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UIG_GridSlot* GridSlot)
		{
			switch (GridSlotState)
			{
			case EIG_GridSlotState::Occupied:
				GridSlot->SetOccupiedTexture();
				break;
			case EIG_GridSlotState::Unoccupied:
				GridSlot->SetUnoccupiedTexture();
				break;
			case EIG_GridSlotState::GreyedOut:
				GridSlot->SetGreyedOutTexture();
				break;
			case EIG_GridSlotState::Selected:
				GridSlot->SetSelectedTexture();
				break;
			}
		}
	);

	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

void UIG_InventoryGrid::PutHoverItemBack()
{
	if (!IsValid(HoverItem)) return;

	FIG_SlotAvailabilityResult Result = HasRoomForItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());
	Result.Item = HoverItem->GetInventoryItem();

	AddStacks(Result);
	ClearHoverItem();
}

void UIG_InventoryGrid::OnInventoryMenuToggled(const bool bOpen)
{
	if (!bOpen)
	{
		PutHoverItemBack();
	}
}

FIG_SlotAvailabilityResult UIG_InventoryGrid::HasRoomForItem(const UIG_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FIG_SlotAvailabilityResult UIG_InventoryGrid::HasRoomForItem(const UIG_InventoryItem* Item, const int32 StackAmountOverride)
{
	return HasRoomForItem(Item->GetItemManifest(), StackAmountOverride);
}

FIG_SlotAvailabilityResult UIG_InventoryGrid::HasRoomForItem(const FIG_ItemManifest& Manifest, const int32 StackAmountOverride)
{
	FIG_SlotAvailabilityResult Result;

	// Determine if the item is stackable
	const FIG_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FIG_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	// Determine how many stacks to add
	const int32 MaxStackSize = Result.bStackable ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = Result.bStackable ? StackableFragment->GetStackCount() : 1;
	if (StackAmountOverride != -1 && Result.bStackable)
	{
		AmountToFill = StackAmountOverride;
	}

	TSet<int32> CheckedIndices;
	// For each Grid Slot:
	for (const auto& GridSlot : GridSlots)
	{
		// If we don't have any more to fill, break out of the loop early
		if (AmountToFill == 0) break;
		
		// Is this index claimed yet?
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetIndex())) continue;

		// Is the item in grid bounds?
		if (!IsInGridBounds(GridSlot->GetIndex(), GetItemDimensions(Manifest))) continue;
		
		// Can the item fit here? (i.e. is it out of grid bounds?)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(Manifest), CheckedIndices, TentativelyClaimed, Manifest.GetItemType(), MaxStackSize))
		{
			continue;
		}
		
		// How much to fill?
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0) continue;
		
		CheckedIndices.Append(TentativelyClaimed);
		
		// Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(FIG_SlotAvailability{
			HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetIndex(),
			Result.bStackable ? AmountToFillInSlot : 0,
			HasValidItem(GridSlot)
		});

		AmountToFill -= AmountToFillInSlot;
		
		// How much is the Remainder?
		Result.Remainder = AmountToFill;
		
		if (AmountToFill == 0) return Result;
	}
	
	return Result;
}

bool UIG_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	return CheckedIndices.Contains(Index);
}

bool UIG_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;
	
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

FIntPoint UIG_InventoryGrid::GetItemDimensions(const FIG_ItemManifest& Manifest) const
{
	const FIG_GridFragment* GridFragment = Manifest.GetFragmentOfType<FIG_GridFragment>();
	return GridFragment? GridFragment->GetGridSize() : FIntPoint(1);
}

bool UIG_InventoryGrid::HasRoomAtIndex(const UIG_GridSlot* GridSlot, const FIntPoint& Dimensions,
	const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize)
{
	// Is there room in this index? (i.e. are there other items in the way?)
	bool bHasRoomAtIndex = true;
	UIG_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetIndex(), Dimensions, Columns, [&](const UIG_GridSlot* SubGridSlot)
		{
			if (CheckSlotConstraints(GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize))
			{
				OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
			}
			else
			{
				bHasRoomAtIndex = false;
			}
		}
	);
	
	return bHasRoomAtIndex;
}

bool UIG_InventoryGrid::CheckSlotConstraints(const UIG_GridSlot* GridSlot, const UIG_GridSlot* SubGridSlot,
	const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{
	// Index claimed?
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetIndex())) return false;
	
	// Has valid item?
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		return true;
	}

	// Is this Grid Slot an upper left slot?
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	
	// If so, is this a stackable item?
	const UIG_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	
	// Is this item the same type as the item we're trying to add?
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;
	
	// If stackable, is this slot at the max stack size already?
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	
	return true;
}

int32 UIG_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UIG_GridSlot* GridSlot) const
{
	// Calculate room in the slot
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	// If stackable, need the minimum between AmountToFill and RoomInSlot
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UIG_InventoryGrid::GetStackAmount(const UIG_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	// If we are at a slot that doesn't hold the stack count, we must get the actual stack count
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		const UIG_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}

	return CurrentSlotStackCount;
}

bool UIG_InventoryGrid::HasValidItem(const UIG_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UIG_InventoryGrid::IsUpperLeftSlot(const UIG_GridSlot* GridSlot, const UIG_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetUpperLeftIndex();
}

bool UIG_InventoryGrid::DoesItemTypeMatch(const UIG_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

void UIG_InventoryGrid::AddItem(UIG_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;

	const FIG_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

bool UIG_InventoryGrid::MatchesCategory(const UIG_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

void UIG_InventoryGrid::AddItemToIndices(const FIG_SlotAvailabilityResult& Result, UIG_InventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
}

void UIG_InventoryGrid::AddItemAtIndex(UIG_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	const FIG_GridFragment* GridFragment = GetFragment<FIG_GridFragment>(Item, FragmentTags::Grid);
	const FIG_ImageFragment* ImageFragment = GetFragment<FIG_ImageFragment>(Item, FragmentTags::Icon);
	if (!GridFragment || !ImageFragment) return;
		
	UIG_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	
	SlottedItems.Add(Index, SlottedItem);
}

void UIG_InventoryGrid::UpdateGridSlots(UIG_InventoryItem* NewItem, const int32 Index, const bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackableItem) GridSlots[Index]->SetStackCount(StackAmount);

	const FIG_GridFragment* GridFragment = GetFragment<FIG_GridFragment>(NewItem, FragmentTags::Grid);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1);
	
	UIG_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UIG_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetUpperLeftIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetAvailable(false);
		}
	);
}

UIG_SlottedItem* UIG_InventoryGrid::CreateSlottedItem(UIG_InventoryItem* Item, const bool bStackable,
	const int32 StackAmount, const FIG_GridFragment* GridFragment, const FIG_ImageFragment* ImageFragment,
	const int32 Index) const
{
	UIG_SlottedItem* SlottedItem = CreateWidget<UIG_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	SlottedItem->UpdateStackCount(bStackable ? StackAmount : 0);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);

	return SlottedItem;
}

void UIG_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FIG_GridFragment* GridFragment, UIG_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChildToCanvas(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos = UIG_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPosWithPadding = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

FVector2D UIG_InventoryGrid::GetDrawSize(const FIG_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2.f;
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UIG_InventoryGrid::SetSlottedItemImage(const UIG_SlottedItem* SlottedItem, const FIG_GridFragment* GridFragment, const FIG_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

bool UIG_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

bool UIG_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

void UIG_InventoryGrid::AddStacks(const FIG_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;

	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UIG_InventoryGrid::OnSlottedItemClicked(const int32 GridIndex, const FPointerEvent& MouseEvent)
{
	UIG_InventoryStatics::ItemUnhovered(GetOwningPlayer());
	
	check(GridSlots.IsValidIndex(GridIndex));
	UIG_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}

	if (IsRightClick(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}

	// Do the hovered item and the clicked inventory item share a type and are they stackable?
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FIG_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FIG_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		// Should we swap their stack counts? (room in the clicked slot == 0 && HoveredStackCount < MaxStackSize)
		if (ShouldSwapStackCounts(RoomInClickedSlot, HoveredStackCount, MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		// Should we consume the hover item's stacks? (room in the clicked slot >= HoveredStackCount)
		if (ShouldConsumeHoverItemStacks(HoveredStackCount, RoomInClickedSlot))
		{
			ConsumeHoverItemStacks(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		// Should we fill in the stacks of the clicked item? (and not consume the hover item)
		if (ShouldFillInStack(RoomInClickedSlot, HoveredStackCount))
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		
		// Clicked slot is already full - do nothing (maybe play a sound?)
		if (RoomInClickedSlot == 0) return;
	}

	// Make sure we can swap with a valid item
	if (CurrentQueryResult.ValidItem.IsValid())
	{
		// Swap with the hover item
		SwapWithHoverItem(ClickedInventoryItem, GridIndex);
	}
}

void UIG_InventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	const UIG_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlots[GridIndex]->GetItemPopUp())) return;

	ItemPopUp = CreateWidget<UIG_ItemPopUp>(this, ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);
	OwningCanvasPanel->AddChildToCanvas(ItemPopUp);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasSlot->SetPosition(MousePosition - ItemPopUpOffset);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());

	const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &ThisClass::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}

	ItemPopUp->OnDrop.BindDynamic(this, &ThisClass::OnPopUpMenuDrop);

	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &ThisClass::OnPopUpMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
}

void UIG_InventoryGrid::PickUp(UIG_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UIG_InventoryGrid::AssignHoverItem(UIG_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UIG_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FIG_GridFragment* GridFragment = GetFragment<FIG_GridFragment>(InventoryItem, FragmentTags::Grid);
	const FIG_ImageFragment* ImageFragment = GetFragment<FIG_ImageFragment>(InventoryItem, FragmentTags::Icon);
	if (!GridFragment || !ImageFragment) return;

	const FVector2D DrawSize = GetDrawSize(GridFragment);
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UIG_InventoryGrid::AssignHoverItem(UIG_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);

	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UIG_InventoryGrid::RemoveItemFromGrid(const UIG_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FIG_GridFragment* GridFragment = GetFragment<FIG_GridFragment>(InventoryItem, FragmentTags::Grid);
	if (!GridFragment) return;

	UIG_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](UIG_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(nullptr);
			GridSlot->SetUpperLeftIndex(INDEX_NONE);
			GridSlot->SetUnoccupiedTexture();
			GridSlot->SetAvailable(true);
			GridSlot->SetStackCount(0);
		}
	);

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UIG_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UIG_InventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}