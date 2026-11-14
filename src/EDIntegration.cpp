#include "EDIntegration.h"
#include "FoundEquipData.h"
#include "Settings.h"

namespace UIIntegration
{
    namespace
    {
        constexpr const char* kColumn = "edHealthColumn";
        constexpr const char* kLayout = "_root.Menu_mc.inventoryLists.itemList.layout";

        RE::GFxValue Object(RE::GFxMovie* movie) {
            RE::GFxValue value;
            movie->CreateObject(&value);
            return value;
        }

        bool Member(const RE::GFxValue& parent, const char* name, RE::GFxValue& value) {
            return parent.IsObject() && parent.GetMember(name, &value) && value.IsObject();
        }

        RE::GFxValue MakeState(RE::GFxMovie* movie, bool descending) {
            auto state = Object(movie);
            auto label = Object(movie);
            label.SetMember("text", "HLTH");
            label.SetMember("arrowDown", descending);
            state.SetMember("label", label);
            auto entry = Object(movie);
            entry.SetMember("text", "@edHealth");
            state.SetMember("entry", entry);

            RE::GFxValue attributes, options;
            movie->CreateArray(&attributes);
            attributes.PushBack("edHealth");
            attributes.PushBack("text");
            movie->CreateArray(&options);
            // ActionScript Array flags: NUMERIC=16, DESCENDING=2, CASEINSENSITIVE=1.
            options.PushBack(descending ? 18 : 16);
            options.PushBack(1);
            state.SetMember("sortAttributes", attributes);
            state.SetMember("sortOptions", options);
            return state;
        }

        bool InsertColumn(RE::GFxValue& columns, const char* after, bool& changed) {
            auto insertion = columns.GetArraySize();
            for (std::uint32_t i = 0; i < columns.GetArraySize(); ++i) {
                RE::GFxValue name;
                if (!columns.GetElement(i, &name) || !name.IsString()) return false;
                if (std::string_view(name.GetString()) == kColumn) return true;
                if (std::string_view(name.GetString()) == after) insertion = i + 1;
            }
            // Mutate the original array so existing references remain valid.
            const RE::GFxValue args[] = { insertion, 0, kColumn };
            if (!columns.Invoke("splice", nullptr, args, 3)) return false;
            changed = true;
            return true;
        }

        class HealthColumnPoll final : public RE::GFxFunctionHandler {
            void Call(Params& params) override {
                auto& scope = *params.thisPtr;
                RE::GFxValue attempts;
                if (!scope.GetMember("attempts", &attempts) || !attempts.IsNumber()) return;
                scope.SetMember("attempts", attempts.GetNumber() + 1);

                RE::GFxValue layout, definitions, views, weapon, armor, weaponColumns, armorColumns;
                const bool ready = params.movie->GetVariable(&layout, kLayout) &&
                    Member(layout, "_columnData", definitions) && Member(layout, "_viewData", views) &&
                    Member(views, "weaponView", weapon) && Member(views, "armorView", armor) &&
                    Member(weapon, "columns", weaponColumns) && weaponColumns.IsArray() &&
                    Member(armor, "columns", armorColumns) && armorColumns.IsArray();
                if (!ready && attempts.GetNumber() < 100) return;

                // The timer and its scope belong to this movie; never retain GFx objects globally.
                RE::GFxValue timer;
                if (scope.GetMember("timer", &timer))
                    params.movie->Invoke("_global.clearInterval", nullptr, &timer, 1);
                if (!ready) {
                    logger::warn("SkyUI health column: compatible inventory layout not available after 5 seconds");
                    return;
                }

                bool changed = false;
                RE::GFxValue existing;
                if (!definitions.GetMember(kColumn, &existing) || existing.IsUndefined() || existing.IsNull()) {
                    auto column = Object(params.movie);
                    column.SetMember("type", 2);
                    column.SetMember("name", "Health");
                    column.SetMember("states", 2);
                    column.SetMember("width", 0.08);
                    column.SetMember("hidden", false);
                    RE::GFxValue damage, border;
                    if (!Member(definitions, "damageColumn", damage) ||
                        !damage.GetMember("border", &border) || !border.IsArray()) {
                        params.movie->CreateArray(&border);
                        border.PushBack(0);
                        border.PushBack(0);
                        border.PushBack(1.1);
                        border.PushBack(0);
                    }
                    column.SetMember("border", border);
                    auto entry = Object(params.movie);
                    auto format = Object(params.movie);
                    format.SetMember("align", "right");
                    entry.SetMember("textFormat", format);
                    column.SetMember("entry", entry);
                    column.SetMember("state1", MakeState(params.movie, true));
                    column.SetMember("state2", MakeState(params.movie, false));
                    if (!definitions.SetMember(kColumn, column)) {
                        logger::warn("SkyUI health column: could not create column definition");
                        return;
                    }
                    changed = true;
                } else if (!existing.IsObject()) {
                    logger::warn("SkyUI health column: existing definition has an unsupported type");
                    return;
                }

                const bool weaponsOK = InsertColumn(weaponColumns, "damageColumn", changed);
                const bool armorOK = InsertColumn(armorColumns, "arColumn", changed);
                RE::GFxValue column, hidden;
                const bool visible = Settings::GetSingleton()->ED_SkyUI_ShowHealth;
                if (Member(definitions, kColumn, column) &&
                    (!column.GetMember("hidden", &hidden) || !hidden.IsBool() || hidden.GetBool() == visible)) {
                    if (column.SetMember("hidden", !visible)) changed = true;
                }
                const bool refreshed = !changed || layout.Invoke("refresh");
                if (!weaponsOK || !armorOK || !refreshed)
                    logger::warn("SkyUI health column: incomplete injection (weapons={}, armor={}, refresh={})", weaponsOK, armorOK, refreshed);
                else
                    logger::info("SkyUI health column: {}", changed ? "injected inventory column" : "existing inventory column preserved");
            }
        };

        bool InitializeHealthColumn(RE::GFxMovieView* view, RE::GFxValue* scope)
        {
            RE::GFxValue url;
            if (!view->GetVariable(&url, "_root._url") || !url.IsString()) return true;
            std::string path(url.GetString());
            std::transform(path.begin(), path.end(), path.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (path.find("inventorymenu.swf") == std::string::npos) return true;

            RE::GFxValue tick;
            RE::GPtr<HealthColumnPoll> handler = RE::make_gptr<HealthColumnPoll>();
            view->CreateFunction(&tick, handler.get());
            scope->SetMember("tick", tick);
            scope->SetMember("attempts", 0);
            const RE::GFxValue args[] = { *scope, "tick", 50 };
            RE::GFxValue timer;
            if (view->Invoke("_global.setInterval", &timer, args, 3) && timer.IsNumber()) {
                scope->SetMember("timer", timer);
                logger::info("SkyUI health column: waiting for inventory layout");
            } else {
                logger::warn("SkyUI health column: could not start inventory layout timer");
            }
            return true;
        }
    }

    void SetHealthColumnVisible(bool visible) {
        Settings::GetSingleton()->ED_SkyUI_ShowHealth = visible;
        // Resolve the current movie inside the task; the inventory may close before it runs.
        if (auto* tasks = SKSE::GetTaskInterface()) {
            tasks->AddUITask([visible] {
                auto* ui = RE::UI::GetSingleton();
                auto menu = ui ? ui->GetMenu<RE::InventoryMenu>() : nullptr;
                if (!menu || !menu->uiMovie) return;
                RE::GFxValue layout, definitions, column;
                if (!menu->uiMovie->GetVariable(&layout, kLayout) ||
                    !Member(layout, "_columnData", definitions) || !Member(definitions, kColumn, column)) return;
                if (!column.SetMember("hidden", !visible) || !layout.Invoke("refresh"))
                    logger::warn("SkyUI health column: could not update visibility");
            });
        }
    }

    void ExtendItemData([[maybe_unused]] RE::GFxMovieView* a_view, RE::GFxValue* a_object, RE::InventoryEntryData* a_item) {
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

    void Register() {
        if (auto* scaleform = SKSE::GetScaleformInterface()) {
            scaleform->Register(ExtendItemData);
            if (!scaleform->Register(InitializeHealthColumn, "EDSHealthColumn"))
                logger::warn("SkyUI health column: could not register Scaleform callback");
        }
    }
}
