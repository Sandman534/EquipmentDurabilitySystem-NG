#pragma once

namespace UIIntegration
{
    void ExtendItemData(RE::GFxMovieView* a_view, RE::GFxValue* a_object, RE::InventoryEntryData* a_item);
    void Register();
    // Applies visibility to an open inventory on the UI thread and future inventories.
    void SetHealthColumnVisible(bool visible);
}
