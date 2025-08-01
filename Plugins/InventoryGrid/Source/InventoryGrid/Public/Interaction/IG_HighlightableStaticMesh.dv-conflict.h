// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IG_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "IG_HighlightableStaticMesh.generated.h"


UCLASS()
class INVENTORYGRID_API UIG_HighlightableStaticMesh : public UStaticMeshComponent, public IIG_Highlightable
{
	GENERATED_BODY()

public:

	/** Highlightable Interface */
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;
	/** end Highlightable Interface */

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;
};
