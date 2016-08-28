set(marble_SRCS qtmain.cpp QtMainWindow.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui)

if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.rc)
endif(MSVC)

qt_wrap_ui(marble_SRCS ${marble_UI})

if (WIN32)
    add_executable (marble-qt WIN32 ${marble_SRCS})
    if (MSVC)
	  set_target_properties(marble-qt PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"")
    endif()
else (WIN32)
    if (APPLE)
        add_executable (marble-qt ${marble_SRCS})
        # For Mac OS X, the executable must be at the root of the bundle's executable folder
        INSTALL(TARGETS marble-qt RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS)
    else (APPLE)
        add_executable (marble-qt ${marble_SRCS})
    endif (APPLE)
endif (WIN32)

target_link_libraries (
marble-qt
Qt5::Widgets
Qt5::PrintSupport
Qt5::Network
marbleui
marblewidget
astro)

if(WIN32)
  target_link_libraries(
    marble-qt
    ws2_32 
    imm32 
    winmm
    )
endif(WIN32)

## this works for win32 only because FindQt does not handle this correct
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
if(APPS_INSTALL_DIR)
  install(PROGRAMS org.kde.marble-qt.desktop DESTINATION ${APPS_INSTALL_DIR})
endif(APPS_INSTALL_DIR)
