#pragma once

#include "NativeGameplayTags.h"

namespace GameItems
{
	namespace Equipment
	{
		namespace Weapons
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Staff);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Knife);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bloodthorn);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(NightsEdge);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shadowbane);
		}

		namespace Cloaks
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlueCloak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(GreenCloak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(GreyCloak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedCloak);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(YellowCloak);
		}

		namespace Masks
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlueMask);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(GreenMask);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(GreyMask);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedMask);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(YellowMask);
		}
	}

	namespace Consumables
	{
		namespace Potions
		{
			namespace Red
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Small);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Large);
			}

			namespace Blue
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Small);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Large);
			}
		}
	}
	
	namespace Craftables
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireFernFruit);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LuminDaisy);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScorchPetalBlossom);
	}
}
