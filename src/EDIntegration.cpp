#include "EDIntegration.h"
#include "FoundEquipData.h"

namespace UIIntegration
{
    void ExtendItemData(RE::GFxMovieView* a_view, RE::GFxValue* a_object, RE::InventoryEntryData* a_item) {
        if (!a_object || !a_item || !a_item->object) return;
        if (!a_item->object->IsArmor() && !a_item->object->IsWeapon()) return;

        FoundEquipData equipData(a_item->object);

        if (a_item->extraLists) {
            for (auto* extraList : *a_item->extraLists) {
                if (!extraList) continue;

                if (auto* extraHealth = extraList->GetByType<RE::ExtraHealth>()) {
                    equipData.objectData = extraList;
                    break;
                }
            }
        }

        a_object->SetMember("edHealth", equipData.GetItemHealthForWidget());
    }

    void Register()
    {
        if (auto* scaleform = SKSE::GetScaleformInterface()) {
            scaleform->Register(ExtendItemData);
        }
    }
}