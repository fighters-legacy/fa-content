// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// The one symbol the engine's ModLoader resolves must be visible outside the
// shared library; everything else stays hidden (CXX_VISIBILITY_PRESET hidden).
#if defined(_WIN32)
#define FA_BRIDGE_EXPORT __declspec(dllexport)
#else
#define FA_BRIDGE_EXPORT __attribute__((visibility("default")))
#endif
