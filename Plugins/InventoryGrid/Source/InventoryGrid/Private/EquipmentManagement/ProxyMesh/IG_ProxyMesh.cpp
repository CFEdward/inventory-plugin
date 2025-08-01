// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/ProxyMesh/IG_ProxyMesh.h"

#include "EquipmentManagement/Components/IG_EquipmentComponent.h"
#include "GameFramework/Character.h"


AIG_ProxyMesh::AIG_ProxyMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(RootComponent);

	EquipmentComponent = CreateDefaultSubobject<UIG_EquipmentComponent>("EquipmentComponent");
	EquipmentComponent->SetOwningSkeletalMesh(MeshComponent);
	EquipmentComponent->SetIsProxy(true);
}

void AIG_ProxyMesh::BeginPlay()
{
	Super::BeginPlay();
	DelayedInitializeOwner();
}

void AIG_ProxyMesh::DelayedInitialization()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::DelayedInitializeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}

void AIG_ProxyMesh::DelayedInitializeOwner()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		DelayedInitialization();
		return;
	}
	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		DelayedInitialization();
		return;
	}
	const ACharacter* Character = Cast<ACharacter>(PC->GetPawn());
	if (!IsValid(Character))
	{
		DelayedInitialization();
		return;
	}
	USkeletalMeshComponent* CharacterMeshComp = Character->GetMesh();
	if (!IsValid(CharacterMeshComp))
	{
		DelayedInitialization();
		return;
	}

	SourceMeshComponent = CharacterMeshComp;
	MeshComponent->SetSkeletalMesh(SourceMeshComponent->GetSkeletalMeshAsset());
	MeshComponent->SetAnimInstanceClass(SourceMeshComponent->GetAnimInstance()->GetClass());

	EquipmentComponent->InitializeOwner(PC);
}
