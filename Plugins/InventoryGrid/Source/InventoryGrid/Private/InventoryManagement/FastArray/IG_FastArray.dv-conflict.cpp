#include "InventoryManagement/FastArray/IG_FastArray.h"

#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "Items/IG_InventoryItem.h"
#include "Items/Components/IG_ItemComponent.h"

void FIG_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	const UIG_InventoryComponent* IC = Cast<UIG_InventoryComponent>(OwnerComponent);
	if (IsValid(IC)) return;

	for (const int32 Index : RemovedIndices)
	{
		IC->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}

void FIG_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	const UIG_InventoryComponent* IC = Cast<UIG_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;

	for (const int32 Index : AddedIndices)
	{
		IC->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

TArray<UIG_InventoryItem*> FIG_InventoryFastArray::GetAllItems() const
{
	TArray<UIG_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		if (!IsValid(Entry.Item)) continue;
		Results.Add(Entry.Item);
	}

	return Results;
}

UIG_InventoryItem* FIG_InventoryFastArray::AddEntry(const UIG_ItemComponent* ItemComponent)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	UIG_InventoryComponent* IC = Cast<UIG_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return nullptr;

	FIG_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);

	IC->AddRepSubObj(NewEntry.Item);
	MarkItemDirty(NewEntry);

	return NewEntry.Item;
}

UIG_InventoryItem* FIG_InventoryFastArray::AddEntry(UIG_InventoryItem* Item)
{
	check(OwnerComponent);
	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FIG_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;
	MarkItemDirty(NewEntry);

	return Item;
}

void FIG_InventoryFastArray::RemoveEntry(UIG_InventoryItem* Item)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FIG_InventoryEntry& Entry = *EntryIt;
		if (Entry.Item == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}
}

UIG_InventoryItem* FIG_InventoryFastArray::FindFirstItemByType(const FGameplayTag& ItemType)
{
	auto* FoundItem = Entries.FindByPredicate([ItemType](const FIG_InventoryEntry& Entry)
		{
			return IsValid(Entry.Item) && Entry.Item->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
		}
	);

	return FoundItem ? FoundItem->Item : nullptr;
}
