cmake_minimum_required(VERSION 3.28)
project(Landcraft VERSION 0.0.1 LANGUAGES CXX)

# --- Runtime link selection ---
if (NOT DEFINED RUNTIME_LINK)
    message(STATUS "RUNTIME_LINK not defined, defaulting to 'dynamic'")
    set(RUNTIME_LINK "dynamic")
endif()
message(STATUS "Runtime link type: ${RUNTIME_LINK}")

# --- Configure MSVC runtime linkage ---
if (MSVC)
    if (RUNTIME_LINK STREQUAL "static")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        message(STATUS "Using static runtime (/MT or /MTd)")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        message(STATUS "Using dynamic runtime (/MD or /MDd)")
    endif()
endif()

# --- Linux static and dynamic linkage settings
if(UNIX AND NOT APPLE)
    if(RUNTIME_LINK STREQUAL "static")
        # enable static linking
        message(STATUS "Linux Static linking")
        set(BUILD_SHARED_LIBS OFF)

        # Link libgcc and libstdc++ statically; however,  system libraries will remain dynamic
        message(STATUS "Using static linkage for libgcc and libstdc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

        # rpath settings to find shared libraries in the executable directory
        set(CMAKE_SKIP_BUILD_RPATH TRUE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        
    else()
        message(STATUS "Linux dynamic linking")
        # portable RPATH
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

        #  disable RUNPATH
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
    endif()
endif()

# --- SFML (via Conan) ---
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")
find_package(SFML REQUIRED COMPONENTS system window graphics)

# --- Output directories ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# --- Executable setup ---
set(MY_TARGET landcraft)
add_executable(${MY_TARGET}
        src/main.cpp
        src/utils/MathUtils.cpp
        src/model/WorldModel.cpp
        src/view/world/WorldView.cpp
        src/controller/WorldController.cpp
        src/view/world/Camera.cpp
        src/model/Tile.cpp
        src/model/TileCorner.cpp
        src/view/world/TileMap.cpp
        src/controller/BrushController.cpp
        src/controller/EditorLoadingController.cpp
        src/view/world/BrushView.cpp
        src/view/world/EnvironmentView.cpp
        src/commands/EditTilesCornersHeightCommand.cpp
        src/commands/PaintTilesCommand.cpp
        src/commands/CommandHistory.cpp
        src/controller/NavigationController.cpp
        src/controller/EditionController.cpp
        src/editionTools/PaintTool.cpp
        src/editionTools/ElevationTool.cpp
        src/LandcraftEditor.cpp
        src/ui/UIFactory.cpp
        src/ui/UIController.cpp
        src/ui/widgets/TextButton.cpp
        src/ui/widgets/SpriteButton.cpp
        src/utils/ResourceManager.cpp
        src/ui/widgets/Box.cpp
        src/ui/widgets/Text.cpp
        src/view/menu/EditionMenu.cpp
        src/ui/widgets/RoundedRectangleShape.cpp
        src/view/menu/BrushMenu.cpp
        src/utils/FileUtils.cpp
        src/controller/MapLoadSaveController.cpp
        src/view/menu/MapSaveLoadMenu.cpp
)

# --- ADDRESS SANITIZER IN DEBUG MODE ---
if(UNIX AND NOT APPLE AND CMAKE_BUILD_TYPE MATCHES Debug)
    # Ajoute les options de compilation pour ASan
    target_compile_options(${MY_TARGET}  PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
    # Ajoute les options de l'éditeur de liens pour ASan
    target_link_options(${MY_TARGET}  PRIVATE -fsanitize=address)
endif()

target_compile_features(${MY_TARGET} PRIVATE cxx_std_17)
target_link_libraries(${MY_TARGET} PUBLIC sfml::sfml)

# --- Install required system libraries for dynamic runtime on Windows ---
if (MSVC AND RUNTIME_LINK STREQUAL "dynamic")
    include(InstallRequiredSystemLibraries)
    install(CODE [[
        include(InstallRequiredSystemLibraries)
        message(STATUS "Copying MSVC runtime for configuration: ${CMAKE_INSTALL_CONFIG_NAME}")
        file(COPY ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "${CMAKE_SOURCE_DIR}/bin")
    ]])
endif()