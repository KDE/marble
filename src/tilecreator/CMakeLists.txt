project( TileCreator )
include_directories(
 ${CMAKE_SOURCE_DIR}/src/tilecreator
 ${CMAKE_BINARY_DIR}/src/tilecreator
 ${QT_INCLUDE_DIR}
)
include( ${QT_USE_FILE} )

set( tilecreator_SRCS
            ../lib/DownloadPolicy.cpp
            ../lib/TileCreator.cpp
            ../lib/TileId.cpp
            ../lib/TileLoaderHelper.cpp
            ../lib/ServerLayout.cpp
            ../lib/geodata/parser/GeoDocument.cpp
            ../lib/geodata/scene/GeoSceneTexture.cpp
            ../lib/geodata/scene/GeoSceneLayer.cpp
            ../lib/MarbleDirs.cpp
            ../lib/MarbleDebug.cpp
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

if(WIN32 AND QTONLY)
    install( TARGETS tilecreator RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
else(WIN32 AND QTONLY)
    if (APPLE AND QTONLY)
      # No need for this when installing to a bundle
    else (APPLE AND QTONLY)
      #set_target_properties(marble PROPERTIES VERSION 1.0.0 SOVERSION 1)
      if( NOT QTONLY)
        install( TARGETS tilecreator ${INSTALL_TARGETS_DEFAULT_ARGS} )
      else( NOT QTONLY)
        install( TARGETS tilecreator RUNTIME DESTINATION bin )
      endif(NOT QTONLY)
    endif (APPLE AND QTONLY)
endif(WIN32 AND QTONLY)

if( TILES_AT_COMPILETIME )
    set(ENV{QT_PLUGIN_PATH} ${QT_PLUGINS_DIR})
    macro( MARBLE_COMPILE_TILES mapname grayscale )
        # the parameters of tilecreator are:
        #  SOURCEDIR  the source directory
        #             this can either be absolute or 
        #             relative to the marble data path.
        #  INSTALLMAP  the map in the form MAPNAME.jpg
        #  DEM  "true" for srtm, "false" else
        #  TARGETDIR  the destination directory
        get_target_property( tilecreator_filename tilecreator LOCATION )
        configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../../data/maps/earth/${mapname}/${mapname}.jpg ${CMAKE_CURRENT_BINARY_DIR}/data/maps/earth/${mapname}/${mapname}.jpg COPYONLY )
        add_custom_command( TARGET tilecreator POST_BUILD
                            COMMAND ${tilecreator_filename} ARGS ${CMAKE_CURRENT_BINARY_DIR}/data/maps/earth/${mapname} ${mapname}.jpg ${grayscale} ${CMAKE_CURRENT_BINARY_DIR}/tiles
                            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                            COMMENT generating tiles for srtm data )
        # this shouldn't happen but it happens far to often
        if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tiles)
            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tiles)
        endif(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tiles)
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tiles/ DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/${mapname})
    endmacro(MARBLE_COMPILE_TILES)
# only for srtm data the generation needs long - for all other data you can also sp
#    MARBLE_COMPILE_TILES(bluemarble false)
#    MARBLE_COMPILE_TILES(citylights false)
#    MARBLE_COMPILE_TILES(plain false)
   MARBLE_COMPILE_TILES(srtm true) # DO NOT comment this line out ever.
                                   # It's essential to keep Marble working.
    
endif( TILES_AT_COMPILETIME )
