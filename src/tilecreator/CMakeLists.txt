project(TileCreator)
#SET(TILES_AT_COMPILETIME 1)
if(TILES_AT_COMPILETIME)
    include_directories(
     ${CMAKE_SOURCE_DIR}/src/tilecreator
     ${CMAKE_BINARY_DIR}/src/tilecreator
     ${QT_INCLUDE_DIR}
     ${QT_QTDESIGNER_INCLUDE_DIR}
    )
    include(${QT_USE_FILE})

    set(tilecreator_SRCS
                ../lib/TileCreator.cpp
                ../lib/TileLoaderHelper.cpp
                ../lib/MarbleDirs.cpp
                main.cpp
                tccore.cpp
    )

    add_definitions( -DMAKE_MARBLE_LIB -DTILECREATOR )
    add_executable(tilecreator ${tilecreator_SRCS})
    target_link_libraries(tilecreator ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

    macro(MARBLE_COMPILE_TILES mapname grayscale)
        # the parameters of tilecreator are:
        #  PREFIX  the source directory
        #  INSTALLMAP  the map in the form MAPNAME/MAPNAME.jpg
        #  DEM  "true" for srtm, "false" else
        #  TARGETDIR the destination directory
        add_custom_command( OUTPUT output1
                            COMMAND tilecreator ARGS "D:/sources/trunk/KDE/kdeedu/marble/data/maps/earth srtm/srtm.jpg true D:/cygopt/kde-root/tmp/marble-qt-0.5.1/work/mingw-Debug/src/tilecreator" )
    endmacro(MARBLE_COMPILE_TILES)
    # only for srtm data the generation needs long - for all other data you can also sp
#    MARBLE_COMPILE_TILES(bluemarble "false")
#    MARBLE_COMPILE_TILES(citylights "false")
#    MARBLE_COMPILE_TILES(plain "false")
#    MARBLE_COMPILE_TILES(srtm "true")
    #install (TARGETS tilecreator RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif(TILES_AT_COMPILETIME)
