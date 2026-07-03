// SPDX-License-Identifier: GPL-3.0-or-later
#include "FaContentPack.h"
#include "PluginExport.h"

// The engine's ModLoader resolves this symbol (IContentPack::kFactorySymbol),
// wraps the returned pointer in std::unique_ptr, and deletes it at shutdown:
// allocation here and deletion there must share one CRT configuration
// (docs/architecture.md, "ABI discipline").
extern "C" FA_BRIDGE_EXPORT fl::IContentPack* fighters_legacy_create_pack() {
    return new fa::FaContentPack();
}
