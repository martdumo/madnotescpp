cmake_minimum_required(VERSION 3.16)

project(MadNotesCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- FASE 1: LA CERCA DE SEGURIDAD ---
if(WIN32)
    # Rutas Windows (Ajustar según tu instalación si cambia)
    # Asumimos que el entorno ya tiene las variables o usas el Developer Command Prompt
    # Pero si quieres forzar rutas como en la guía:
    # set(CMAKE_PREFIX_PATH "F:/Qt/6.x.x/msvc2019_64" CACHE PATH "") 
else()
    # Linux (Arch/Ubuntu) busca en /usr/lib/cmake automágicamente
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(include include/core include/ui include/editor include/utils)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/ui/*.cpp" "src/editor/*.cpp" "src/core/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/ui/*.h" "include/editor/*.h" "include/core/*.h")

# Recursos (Iconos)
set(APP_ICON_RESOURCE "madnotes.rc")
set(QRC_RESOURCES "resources.qrc")

# Executable
if(WIN32)
    add_executable(MadNotesCpp WIN32 ${SOURCES} ${HEADERS} ${APP_ICON_RESOURCE} ${QRC_RESOURCES})
else()
    add_executable(MadNotesCpp ${SOURCES} ${HEADERS} ${QRC_RESOURCES})
endif()

target_link_libraries(MadNotesCpp PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network)

# --- INSTALACIÓN (Linux 'make install') ---
if(UNIX AND NOT APPLE)
    install(TARGETS MadNotesCpp DESTINATION bin)
    install(FILES resources/app.png DESTINATION share/icons/hicolor/256x256/apps RENAME madnotes.png)
    # Aquí se podría generar el .desktop file también
endif()
