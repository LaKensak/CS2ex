cmake_minimum_required(VERSION 3.10)

# Nom du projet
project(CS2Cheat)

# Définir le standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de compilation
if(MSVC)
    add_compile_options(/W4 /MT)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Sous-projet pour l'injecteur
add_executable(Injector 
    Injector/main.cpp
)

# Sous-projet pour le DLL du cheat
add_library(CS2Cheat SHARED
    CS2Cheat/dllmain.cpp
    CS2Cheat/Memory.cpp
    CS2Cheat/Features.cpp
    CS2Cheat/Utils.cpp
    CS2Cheat/Hooks.cpp
    CS2Cheat/GUI.cpp
)

# Inclure les répertoires d'en-tête
target_include_directories(CS2Cheat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CS2Cheat
)

target_include_directories(Injector PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Injector
)

# Ajouter les bibliothèques nécessaires
target_link_libraries(CS2Cheat PRIVATE
    d3d11.lib
    dxgi.lib
)

# Configuration de post-build pour copier l'injecteur et le DLL dans le même répertoire
add_custom_command(TARGET Injector POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Injector> ${CMAKE_BINARY_DIR}/bin/
)

add_custom_command(TARGET CS2Cheat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CS2Cheat> ${CMAKE_BINARY_DIR}/bin/
)