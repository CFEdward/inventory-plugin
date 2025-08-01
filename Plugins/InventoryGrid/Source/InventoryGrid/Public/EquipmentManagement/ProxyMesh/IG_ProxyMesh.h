// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IG_ProxyMesh.generated.h"

class UIG_EquipmentComponent;

UCLASS()
class INVENTORYGRID_API AIG_ProxyMesh : public AActor
{
	GENERATED_BODY()

public:
	
	AIG_ProxyMesh();

	USkeletalMeshComponent* GetMeshComponent() const { return MeshComponent; }

protected:
	
	virtual void BeginPlay() override;

private:

	/** This is the mesh on the player-controlled Character */
	TWeakObjectPtr<USkeletalMeshComponent> SourceMeshComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UIG_EquipmentComponent> EquipmentComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	FTimerHandle TimerForNextTick;
	void DelayedInitialization();
	void DelayedInitializeOwner();
};
