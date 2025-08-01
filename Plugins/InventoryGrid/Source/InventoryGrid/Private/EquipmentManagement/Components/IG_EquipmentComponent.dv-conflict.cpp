// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/Components/IG_EquipmentComponent.h"

#include "EquipmentManagement/EquipActor/IG_EquipActor.h"
#include "GameFramework/Character.h"
#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "InventoryManagement/Utils/IG_InventoryStatics.h"
#include "Items/IG_InventoryItem.h"
#include "Items/Fragments/IG_ItemFragment.h"

void UIG_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPlayerController();
}

void UIG_EquipmentComponent::InitPlayerController()
{
	if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
	{
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwnerCharacter))
		{
			OnPossessedPawnChange(nullptr, OwnerCharacter);
		}
		else
		{
			OwningPlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChange);
		}
	}
}

void UIG_EquipmentComponent::OnPossessedPawnChange(APawn* OldPawn, APawn* NewPawn)
{
	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwnerCharacter))
	{
		OwningSkeletalMesh = OwnerCharacter->GetMesh();
	}
	InitInventoryComponent();
}

void UIG_EquipmentComponent::InitializeOwner(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		OwningPlayerController = PlayerController;
	}
	InitInventoryComponent();
}

void UIG_EquipmentComponent::InitInventoryComponent()
{
	InventoryComponent = UIG_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
	if (!InventoryComponent.IsValid()) return;

	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
	}
	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}
}

AIG_EquipActor* UIG_EquipmentComponent::SpawnEquippedActor(FIG_EquipmentFragment* EquipmentFragment, const FIG_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh) const
{
	AIG_EquipActor* SpawnedEquipActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
	SpawnedEquipActor->SetEquipmentType(EquipmentFragment->GetEquipmentType());
	SpawnedEquipActor->SetOwner(GetOwner());
	EquipmentFragment->SetEquippedActor(SpawnedEquipActor);

	return SpawnedEquipActor;
}

AIG_EquipActor* UIG_EquipmentComponent::FindEquippedActorByTag(const FGameplayTag& EquipmentTypeTag)
{
	auto* FoundActor = EquippedActors.FindByPredicate([&EquipmentTypeTag](const AIG_EquipActor* EquippedActor)
		{
			return EquippedActor->GetEquipmentType().MatchesTagExact(EquipmentTypeTag);
		}
	);

	return FoundActor ? *FoundActor : nullptr;
}

void UIG_EquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
	if (AIG_EquipActor* EquippedActor = FindEquippedActorByTag(EquipmentTypeTag); IsValid(EquippedActor))
	{
		EquippedActors.Remove(EquippedActor);
		EquippedActor->Destroy();
	}
}

void UIG_EquipmentComponent::OnItemEquipped(UIG_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem) || !OwningPlayerController->HasAuthority()) return;
	
	FIG_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FIG_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FIG_EquipmentFragment>();
	if (!EquipmentFragment) return;

	if (!bIsProxy) EquipmentFragment->OnEquip(OwningPlayerController.Get());

	if (!OwningSkeletalMesh.IsValid()) return;
	AIG_EquipActor* SpawnedEquipActor = SpawnEquippedActor(EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());
	EquippedActors.Add(SpawnedEquipActor);
}

void UIG_EquipmentComponent::OnItemUnequipped(UIG_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem) || !OwningPlayerController->HasAuthority()) return;
	
	FIG_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FIG_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FIG_EquipmentFragment>();
	if (!EquipmentFragment) return;

	if (!bIsProxy) EquipmentFragment->OnUnequip(OwningPlayerController.Get());

	RemoveEquippedActor(EquipmentFragment->GetEquipmentType());
}

void UIG_EquipmentComponent::SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh)
{
	OwningSkeletalMesh = OwningMesh;
}
