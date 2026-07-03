# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(test_fa_content_pack test_fa_content_pack.cpp)
target_link_libraries(test_fa_content_pack PRIVATE fa-bridge-core)
add_test(NAME fa_content_pack COMMAND test_fa_content_pack)

# Loads the built plugin exactly as the engine's ModLoader does. Paths travel
# as arguments, not compile definitions — Windows paths in compile definitions
# are a backslash-escape hazard.
add_executable(test_plugin_load test_plugin_load.cpp)
target_link_libraries(test_plugin_load PRIVATE fl-headers ${CMAKE_DL_LIBS})
add_dependencies(test_plugin_load fa-bridge)
add_test(NAME plugin_load
         COMMAND test_plugin_load $<TARGET_FILE:fa-bridge>
                 ${CMAKE_BINARY_DIR}/stage/mods/fa-bridge/manifest.toml)
