// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/IG_GridTypes.h"
#include "Widgets/Utils/IG_WidgetUtils.h"
#include "IG_InventoryStatics.generated.h"

class UIG_InventoryBase;
class UIG_HoverItem;
class UIG_ItemComponent;
class UIG_InventoryComponent;

UCLASS()
class INVENTORYGRID_API UIG_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static UIG_InventoryBase* GetInventoryWidget(APlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemHovered(const APlayerController* PC, UIG_InventoryItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemUnhovered(const APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UIG_HoverItem* GetHoverItem(APlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UIG_InventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EIG_ItemCategory GetItemCategoryFromItemComp(const UIG_ItemComponent* ItemComp);

	template<typename T, typename FuncT>
	static void ForEach2D(TArray<T>& Array, const int32 Index, const FIntPoint& Range2D, const int32 GridColumns, const FuncT& Function);
};

template<typename T, typename FuncT>
void UIG_InventoryStatics::ForEach2D(TArray<T>& Array, const int32 Index, const FIntPoint& Range2D, const int32 GridColumns, const FuncT& Function)
{
	for (int32 j = 0; j < Range2D.Y; ++j)
	{
		for (int32 i = 0; i < Range2D.X; ++i)
		{
			const FIntPoint Coordinates = UIG_WidgetUtils::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i, j);
			const int32 TileIndex = UIG_WidgetUtils::GetIndexFromPosition(Coordinates, GridColumns);
			if (Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}