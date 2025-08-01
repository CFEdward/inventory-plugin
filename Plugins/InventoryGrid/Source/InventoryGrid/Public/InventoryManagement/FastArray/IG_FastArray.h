#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "IG_FastArray.generated.h"

struct FGameplayTag;
class UIG_ItemComponent;
class UIG_InventoryComponent;
class UIG_InventoryItem;

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FIG_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	
	FIG_InventoryEntry() {}

private:

	friend struct FIG_InventoryFastArray;
	friend UIG_InventoryComponent;
	
	UPROPERTY()
	TObjectPtr<UIG_InventoryItem> Item{nullptr};
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FIG_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

public:

	FIG_InventoryFastArray() : OwnerComponent(nullptr) {}
	FIG_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UIG_InventoryItem*> GetAllItems() const;

	/** FFastArraySerializer contract */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	/** end FFastArraySerializer contract */

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FIG_InventoryEntry, FIG_InventoryFastArray>(Entries, DeltaParams, *this);
	}

	UIG_InventoryItem* AddEntry(const UIG_ItemComponent* ItemComponent);
	UIG_InventoryItem* AddEntry(UIG_InventoryItem* Item);
	void RemoveEntry(UIG_InventoryItem* Item);
	UIG_InventoryItem* FindFirstItemByType(const FGameplayTag& ItemType);
	
private:

	friend UIG_InventoryComponent;

	/** Replicated list of items */
	UPROPERTY()
	TArray<FIG_InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FIG_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FIG_InventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};