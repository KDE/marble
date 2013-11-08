set(marble_SRCS qtmain.cpp QtMainWindow.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui)

if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.rc)
endif(MSVC)

if(MINGW)
   QT4_ADD_RESOURCES2(marble_SRCS marble.rc)
endif(MINGW)

qt_wrap_ui(marble_SRCS ${marble_UI})
marble_qt4_automoc(${marble_SRCS})

if (WIN32)
    add_executable (marble-qt WIN32 ${marble_SRCS})
else (WIN32)
    if (APPLE)
        add_executable (marble-qt MACOSX_BUNDLE ${marble_SRCS})
        # For Mac OS X, the executable must be at the root of the bundle's executable folder
        INSTALL(TARGETS marble-qt RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
    else (APPLE)
        add_executable (marble-qt ${marble_SRCS})
    endif (APPLE)
endif (WIN32)

if (QT4_FOUND)
  target_link_libraries (
    marble-qt
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTSVG_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
    marbleui
    marblewidget)
else()
  target_link_libraries (
    marble-qt
    ${Qt5Core_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${Qt5WebKitWidgets_LIBRARIES}
    ${Qt5WebKit_LIBRARIES}
    marbleui
    marblewidget)
endif()

if(WIN32)
  target_link_libraries(
    marble-qt
    ws2_32 
    imm32 
    winmm
    )
endif(WIN32)

## this works for win32 only because FindQt4 does not handle this correct
if(STATIC_BUILD)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)

if(WIN32)
install (TARGETS marble-qt RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
else(WIN32)
if (APPLE)
  # No need for this when installing to a bundle
else (APPLE)
    install (TARGETS marble-qt RUNTIME DESTINATION bin )
endif (APPLE)
endif(WIN32)

install(PROGRAMS marble-qt.desktop DESTINATION ${APPS_INSTALL_DIR})
