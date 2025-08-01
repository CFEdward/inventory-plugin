#pragma once

#include "IG_GridTypes.generated.h"

class UIG_InventoryItem;

UENUM(BlueprintType)
enum class EIG_ItemCategory : uint8
{
	Equippable,
	Consumable,
	Craftable,
	None
};

USTRUCT()
struct FIG_SlotAvailability
{
	GENERATED_BODY()

public:

	FIG_SlotAvailability() {}
	FIG_SlotAvailability(const int32 ItemIndex, const int32 Room, const bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}
	
	int32 Index{INDEX_NONE};
	int32 AmountToFill{0};
	bool bItemAtIndex{false};
};

USTRUCT()
struct FIG_SlotAvailabilityResult
{
	GENERATED_BODY()

public:
	
	FIG_SlotAvailabilityResult() {}

	TWeakObjectPtr<UIG_InventoryItem> Item;
	int32 TotalRoomToFill{0};
	int32 Remainder{0};
	bool bStackable{false};
	TArray<FIG_SlotAvailability> SlotAvailabilities;
};

UENUM(BlueprintType)
enum class EIG_TileQuadrant : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	None
};

USTRUCT(BlueprintType)
struct FIG_TileParameters
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint TileCoordinates{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{INDEX_NONE};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	EIG_TileQuadrant TileQuadrant{EIG_TileQuadrant::None};
};

inline bool operator==(const FIG_TileParameters& A, const FIG_TileParameters& B)
{
	return A.TileCoordinates == B.TileCoordinates && A.TileIndex == B.TileIndex && A.TileQuadrant == B.TileQuadrant;
}

USTRUCT()
struct FIG_SpaceQueryResult
{
	GENERATED_BODY()

public:

	/** True if the space queried has no items in it */
	bool bHasSpace{false};
	/** Valid if there's a single item we can swap with */
	TWeakObjectPtr<UIG_InventoryItem> ValidItem{nullptr};
	/** Upper left index of the valid item, if there is one */
	int32 UpperLeftIndex{INDEX_NONE};
};