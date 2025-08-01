// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IG_CompositeBase.h"
#include "IG_Composite.generated.h"

UCLASS()
class INVENTORYGRID_API UIG_Composite : public UIG_CompositeBase
{
	GENERATED_BODY()

public:

	virtual void ApplyFunction(FuncType Function) override;

	virtual void Collapse() override;

	TArray<TObjectPtr<UIG_CompositeBase>> GetChildren() const { return Children;}
	
protected:

	virtual void NativeOnInitialized() override;
	
private:

	UPROPERTY()
	TArray<TObjectPtr<UIG_CompositeBase>> Children;
};
