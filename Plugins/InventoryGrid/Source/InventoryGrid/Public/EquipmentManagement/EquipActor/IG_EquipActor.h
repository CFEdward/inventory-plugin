// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "IG_EquipActor.generated.h"

UCLASS()
class INVENTORYGRID_API AIG_EquipActor : public AActor
{
	GENERATED_BODY()

public:
	
	AIG_EquipActor();

	FGameplayTag GetEquipmentType() const { return EquipmentType; }
	void SetEquipmentType(const FGameplayTag Type) { EquipmentType = Type; }
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems.Equipment"))
	FGameplayTag EquipmentType;
};
