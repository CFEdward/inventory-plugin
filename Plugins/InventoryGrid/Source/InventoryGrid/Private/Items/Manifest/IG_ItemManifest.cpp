#include "Items/Manifest/IG_ItemManifest.h"

#include "Items/IG_InventoryItem.h"
#include "Items/Components/IG_ItemComponent.h"
#include "Items/Fragments/IG_ItemFragment.h"
#include "Widgets/Composite/IG_CompositeBase.h"

UIG_InventoryItem* FIG_ItemManifest::Manifest(UObject* NewOuter)
{
	UIG_InventoryItem* Item = NewObject<UIG_InventoryItem>(NewOuter, UIG_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	for (auto& Fragment : Item->GetItemManifestMutable().GetFragmentsMutable())
	{
		Fragment.GetMutable().Manifest();
	}
	ClearFragments();

	return Item;
}

void FIG_ItemManifest::AssimilateInventoryFragments(UIG_CompositeBase* Composite) const
{
	const auto& InventoryItemFragments = GetAllFragmentsOfType<FIG_WidgetItemFragment>();
	for (const auto* Fragment : InventoryItemFragments)
	{
		Composite->ApplyFunction([Fragment](UIG_CompositeBase* Widget)
			{
				Fragment->Assimilate(Widget);
			}
		);
	}
}

void FIG_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;

	const AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;

	// Set the item manifest, item category, item type, etc.
	UIG_ItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UIG_ItemComponent>();
	check(ItemComp);
	
	ItemComp->InitItemManifest(*this);
}

void FIG_ItemManifest::ClearFragments()
{
	for (auto& Fragment : Fragments)
	{
		Fragment.Reset();
	}
}
