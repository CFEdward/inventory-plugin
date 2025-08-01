#include "Items/Fragments/IG_FragmentTags.h"

namespace FragmentTags
{
	UE_DEFINE_GAMEPLAY_TAG(Grid, "FragmentTags.Grid");
	UE_DEFINE_GAMEPLAY_TAG(Icon, "FragmentTags.Icon");
	UE_DEFINE_GAMEPLAY_TAG(Stackable, "FragmentTags.Stackable");
	UE_DEFINE_GAMEPLAY_TAG(Consumable, "FragmentTags.Consumable");
	UE_DEFINE_GAMEPLAY_TAG(Equipment, "FragmentTags.Equipment");
	
	UE_DEFINE_GAMEPLAY_TAG(ItemName, "FragmentTags.ItemName");
	UE_DEFINE_GAMEPLAY_TAG(ItemType, "FragmentTags.ItemType");
	UE_DEFINE_GAMEPLAY_TAG(FlavorText, "FragmentTags.FlavorText");
	UE_DEFINE_GAMEPLAY_TAG(SellValue, "FragmentTags.SellValue");
	UE_DEFINE_GAMEPLAY_TAG(RequiredLevel, "FragmentTags.RequiredLevel");

	namespace StatMod
	{
		UE_DEFINE_GAMEPLAY_TAG(PrimaryStat, "FragmentTags.StatMod.PrimaryStat");
		UE_DEFINE_GAMEPLAY_TAG(StatMod_1, "FragmentTags.StatMod.1");
		UE_DEFINE_GAMEPLAY_TAG(StatMod_2, "FragmentTags.StatMod.2");
		UE_DEFINE_GAMEPLAY_TAG(StatMod_3, "FragmentTags.StatMod.3");
	}
}