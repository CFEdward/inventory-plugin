// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/IG_InventoryComponent.h"

#include "Items/Components/IG_ItemComponent.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "Items/IG_InventoryItem.h"
#include "Items/Fragments/IG_ItemFragment.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/InventoryBase/IG_InventoryBase.h"

UIG_InventoryComponent::UIG_InventoryComponent() : InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UIG_InventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

void UIG_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory();
}

void UIG_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("Inventory Component should have a Player Controller as Owner."));
	if (!OwningController->IsLocalController()) return;

	InventoryMenu = CreateWidget<UIG_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UIG_InventoryComponent::TryAddItem(UIG_ItemComponent* ItemComponent)
{
	FIG_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);

	UIG_InventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;

	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		// Add stacks to an item that already exists in the inventory. We only want to update the stack count
		// not create a new item of this type
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else if (Result.TotalRoomToFill > 0)
	{
		// This item type doesn't exist in the inventory. Create a new one and update all pertinent slots
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 0);
	}
}

void UIG_InventoryComponent::Server_AddNewItem_Implementation(UIG_ItemComponent* ItemComponent, const int32 StackCount)
{
	UIG_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}
	
	ItemComponent->PickedUp();
}

void UIG_InventoryComponent::Server_AddStacksToItem_Implementation(UIG_ItemComponent* ItemComponent, const int32 StackCount, const int32 Remainder)
{
	const FGameplayTag& ItemType = IsValid(ItemComponent) ? ItemComponent->GetItemManifest().GetItemType() : FGameplayTag::EmptyTag;
	UIG_InventoryItem* Item = InventoryList.FindFirstItemByType(ItemType);
	if (!IsValid(Item)) return;

	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	else if (FIG_StackableFragment* StackableFragment = ItemComponent->GetItemManifest().GetFragmentOfTypeMutable<FIG_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

void UIG_InventoryComponent::Server_DropItem_Implementation(UIG_InventoryItem* Item, const int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	SpawnDroppedItem(Item, StackCount);
}

void UIG_InventoryComponent::SpawnDroppedItem(UIG_InventoryItem* Item, const int32 StackCount) const
{
	const APawn* OwningPawn = OwningController->GetPawn();
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
	SpawnLocation.Z -= RelativeSpawnElevation;

	FIG_ItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FIG_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FIG_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnPickupActor(this, SpawnLocation, FRotator::ZeroRotator);
}

void UIG_InventoryComponent::Server_ConsumeItem_Implementation(UIG_InventoryItem* Item)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - 1;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	if (FIG_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FIG_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get());
	}
}

void UIG_InventoryComponent::Server_EquipSlotClicked_Implementation(UIG_InventoryItem* ItemToEquip, UIG_InventoryItem* ItemToUnequip)
{
	Multicast_EquipSlotClicked(ItemToEquip, ItemToUnequip);
}

void UIG_InventoryComponent::Multicast_EquipSlotClicked_Implementation(UIG_InventoryItem* ItemToEquip, UIG_InventoryItem* ItemToUnequip)
{
	// Equipment Component will listen to these delegates
	OnItemEquipped.Broadcast(ItemToEquip);
	OnItemUnequipped.Broadcast(ItemToUnequip);
}

void UIG_InventoryComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen) CloseInventoryMenu();
	else OpenInventoryMenu();
	OnInventoryMenuToggled.Broadcast(bInventoryMenuOpen);
}

void UIG_InventoryComponent::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
		AddReplicatedSubObject(SubObj);
}

void UIG_InventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return;

	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen = true;

	if (!OwningController.IsValid()) return;

	const FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);
}

void UIG_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return;

	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	bInventoryMenuOpen = false;

	if (!OwningController.IsValid()) return;

	const FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);
}
