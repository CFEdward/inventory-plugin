// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/IG_ItemComponent.h"

#include "Net/UnrealNetwork.h"


UIG_ItemComponent::UIG_ItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PickupMessage = FString("E - Pick Up");
}

void UIG_ItemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		ItemManifest.SetPickupActorClass(GetOwner()->GetClass());
	}
}

void UIG_ItemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ItemManifest);
}

void UIG_ItemComponent::PickedUp()
{
	OnPickedUp();
	GetOwner()->Destroy();
}

void UIG_ItemComponent::InitItemManifest(FIG_ItemManifest CopyOfManifest)
{
	ItemManifest = CopyOfManifest;
	ItemManifest.SetPickupActorClass(GetOwner()->GetClass());
}
