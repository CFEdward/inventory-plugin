#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "Types/IG_GridTypes.h"
#include "IG_ItemManifest.generated.h"

class UIG_CompositeBase;
struct FIG_ItemFragment;

/*
 * The Item Manifest contains all the necessary data
 * for creating a new Inventory Item
 */
USTRUCT(BlueprintType)
struct INVENTORYGRID_API FIG_ItemManifest
{
	GENERATED_BODY()

public:

	UIG_InventoryItem* Manifest(UObject* NewOuter);

	void AssimilateInventoryFragments(UIG_CompositeBase* Composite) const;

	void SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
	EIG_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }
	TArray<TInstancedStruct<FIG_ItemFragment>>& GetFragmentsMutable() { return Fragments; }
	TSubclassOf<AActor> GetPickupActorClass() const { return PickupActorClass; }
	void SetPickupActorClass(const TSubclassOf<AActor>& Actor) { PickupActorClass = Actor; }

	template<typename T> requires std::derived_from<T, FIG_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;
	template<typename T> requires std::derived_from<T, FIG_ItemFragment>
	const T* GetFragmentOfType() const;
	template<typename T> requires std::derived_from<T, FIG_ItemFragment>
	T* GetFragmentOfTypeMutable();
	template<typename T> requires std::derived_from<T, FIG_ItemFragment>
	TArray<const T*> GetAllFragmentsOfType() const;
	
private:

	void ClearFragments();
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TSubclassOf<AActor> PickupActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FIG_ItemFragment>> Fragments;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EIG_ItemCategory ItemCategory{EIG_ItemCategory::None};

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems"))
	FGameplayTag ItemType;
};

template<typename T> requires std::derived_from<T, FIG_ItemFragment>
const T* FIG_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const TInstancedStruct<FIG_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue;
			return FragmentPtr;
		}
	}
	
	return nullptr;
}

template <typename T> requires std::derived_from<T, FIG_ItemFragment>
const T* FIG_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FIG_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	
	return nullptr;
}

template<typename T> requires std::derived_from<T, FIG_ItemFragment>
T* FIG_ItemManifest::GetFragmentOfTypeMutable()
{
	for (TInstancedStruct<FIG_ItemFragment>& Fragment : Fragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}
	
	return nullptr;
}

template <typename T> requires std::derived_from<T, FIG_ItemFragment>
TArray<const T*> FIG_ItemManifest::GetAllFragmentsOfType() const
{
	TArray<const T*> Result;
	for (const TInstancedStruct<FIG_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			Result.Add(FragmentPtr);
		}
	}
	
	return Result;
}
