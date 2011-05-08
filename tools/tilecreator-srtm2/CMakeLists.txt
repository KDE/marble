project( TileCreatorSrtm2 )

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

include_directories(
 ${CMAKE_SOURCE_DIR}
 ${CMAKE_BINARY_DIR}
 ../../src/lib
 ../../src/lib/geodata
 ../../src/lib/geodata/scene
 ../../src/lib/geodata/parser
 ${CMAKE_BINARY_DIR}/../../src
 ${QT_INCLUDE_DIR}
)
include( ${QT_USE_FILE} )

set( tilecreator_SRCS
            ../../src/lib/DownloadPolicy.cpp
            ../../src/lib/TileCreator.cpp
            ../../src/lib/TileId.cpp
            ../../src/lib/TileLoaderHelper.cpp
            ../../src/lib/ServerLayout.cpp
            ../../src/lib/geodata/parser/GeoDocument.cpp
            ../../src/lib/geodata/scene/GeoSceneTexture.cpp
            ../../src/lib/geodata/scene/GeoSceneLayer.cpp
            ../../src/lib/geodata/parser/GeoSceneTypes.cpp
            ../../src/lib/MarbleDirs.cpp
            ../../src/lib/MarbleDebug.cpp
            main.cpp
            tccore.cpp
)

qt4_automoc(${tilecreator_SRCS})

add_definitions( -DMAKE_MARBLE_LIB -DTILECREATOR )
add_executable( tilecreator ${tilecreator_SRCS} )
target_link_libraries( tilecreator ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} )
if (APPLE)
  target_link_libraries (tilecreator ${APP_SERVICES_LIBRARY})
endif (APPLE)
