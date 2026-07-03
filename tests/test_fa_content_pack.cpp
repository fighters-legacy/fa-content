// SPDX-License-Identifier: GPL-3.0-or-later

// Assert-based smoke test for the stub pack (the framework choice is a
// Phase 2 decision, issue #38). Asserts must fire in every configuration.
#undef NDEBUG

#include "FaContentPack.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace {

void setEnv(const char* name, const char* value) {
#if defined(_WIN32)
    _putenv_s(name, value);
#else
    setenv(name, value, 1);
#endif
}

void unsetEnv(const char* name) {
#if defined(_WIN32)
    _putenv_s(name, "");
#else
    unsetenv(name);
#endif
}

} // namespace

int main() {
    fa::FaContentPack pack;

    // Identity
    assert(std::strcmp(pack.id(), "fa-bridge") == 0);
    assert(std::strcmp(pack.name(), "Fighters Anthology Bridge") == 0);
    assert(pack.version() != nullptr && *pack.version() != '\0');
    assert(pack.priority() == 100);
    assert(pack.priority() > 50); // must override fl-base-pack in the stack
    assert(pack.rootDirectory() == nullptr);
    assert(pack.getTrustLevel() == fl::TrustLevel::Unsigned);
    assert(pack.isNativePlugin());

    // Stub behaviour: nothing is served yet
    assert(!pack.configure(nullptr));
    assert(!pack.loadMesh("any"));
    assert(!pack.loadTexture("any"));
    assert(!pack.loadAudio("any"));
    assert(!pack.loadFlightModel("any"));
    assert(!pack.loadMission("any"));
    assert(!pack.loadTerrain("any"));
    assert(!pack.loadAIScript("any"));
    assert(!pack.loadEntityDef("any"));
    assert(!pack.loadConfig("difficulty.toml"));
    assert(!pack.resolveTerrainChunk("world", 0, 0, 0));

    for (uint8_t t = 0; t < static_cast<uint8_t>(fl::AssetType::Count); ++t) {
        const auto type = static_cast<fl::AssetType>(t);
        assert(!pack.hasAsset("any", type));
        assert(pack.listAssets(type).empty());
    }

    // init(): env unset → NeedsConfiguration
    unsetEnv("FA_INSTALL_DIR");
    assert(pack.init() == fl::IContentPack::Status::NeedsConfiguration);

    // init(): non-existent path → NeedsConfiguration
    const auto missing = std::filesystem::temp_directory_path() / "fa-bridge-test-does-not-exist";
    std::filesystem::remove_all(missing);
    setEnv("FA_INSTALL_DIR", missing.string().c_str());
    assert(pack.init() == fl::IContentPack::Status::NeedsConfiguration);

    // init(): existing *file* → NeedsConfiguration (is_directory, not exists)
    const auto file = std::filesystem::temp_directory_path() / "fa-bridge-test-file";
    std::ofstream(file).put('x');
    setEnv("FA_INSTALL_DIR", file.string().c_str());
    assert(pack.init() == fl::IContentPack::Status::NeedsConfiguration);
    std::filesystem::remove(file);

    // init(): existing directory → Ready, and idempotent
    const auto dir = std::filesystem::temp_directory_path() / "fa-bridge-test-dir";
    std::filesystem::create_directories(dir);
    setEnv("FA_INSTALL_DIR", dir.string().c_str());
    assert(pack.init() == fl::IContentPack::Status::Ready);
    assert(pack.init() == fl::IContentPack::Status::Ready);
    std::filesystem::remove_all(dir);

    unsetEnv("FA_INSTALL_DIR");
    std::puts("test_fa_content_pack: OK");
    return 0;
}
