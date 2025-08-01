// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IG_CompositeBase.h"
#include "IG_Leaf.generated.h"

UCLASS()
class INVENTORYGRID_API UIG_Leaf : public UIG_CompositeBase
{
	GENERATED_BODY()

public:

	virtual void ApplyFunction(FuncType Function) override;
};
