set(marble_kde_SRCS 
  kdemain.cpp 
  KdeMainWindow.cpp 
  marble_part.cpp
)

set(marblepart_SRCS marble_part.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui ${KDE4_INCLUDE_DIR})
kde4_add_kcfg_files(marblepart_SRCS settings.kcfgc)
kde4_add_plugin(marble_part WITH_PREFIX ${marblepart_SRCS})
target_link_libraries(marble_part ${KDE4_KPARTS_LIBS} ${KDE4_KNEWSTUFF3_LIBS} marbleui marblewidget)

install(TARGETS marble_part DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES marble_part.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES marble_part.rc DESTINATION ${DATA_INSTALL_DIR}/marble)

kde4_add_ui_files(marble_kde_SRCS ${marble_UI})
kde4_add_kcfg_files(marble_kde_SRCS settings.kcfgc)

kde4_add_executable (marble ${marble_kde_SRCS})

target_link_libraries (
    marble
    ${KDE4_KPARTS_LIBS} 
    ${KDE4_KNEWSTUFF3_LIBS}
    marbleui
    marblewidget
    )

## this works for win32 only because FindQt4 does not handle this correct
if(STATIC_BUILD)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)

if (NOT APPLE)
    install( FILES marble.desktop DESTINATION ${APPS_INSTALL_DIR})
endif()

install(TARGETS marble ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS marble.desktop DESTINATION ${APPS_INSTALL_DIR})
install(FILES marble.kcfg DESTINATION ${KCFG_INSTALL_DIR})

# install the XML GUI file and the Hot New Stuff config
install(FILES marbleui.rc DESTINATION ${DATA_INSTALL_DIR}/marble)
install(FILES marble.knsrc DESTINATION ${DATA_INSTALL_DIR}/marble)
