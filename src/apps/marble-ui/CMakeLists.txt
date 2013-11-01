set(marble_SRCS ControlView.cpp MarbleTest.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.rc)
endif(MSVC)

if(MINGW)
   QT4_ADD_RESOURCES2(marble_SRCS marble.rc)
endif(MINGW)

qt_add_resources(marble_SRCS marble.qrc)

qt_wrap_ui(marble_SRCS ${marble_UI})
marble_qt4_automoc(${marble_SRCS})

add_library (marbleui STATIC ${marble_SRCS})

if( QT4_FOUND )
target_link_libraries (
  marbleui
  ${QT_QTCORE_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTXML_LIBRARY}
  ${QT_QTSVG_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  marblewidget)
else()
target_link_libraries (
    marbleui
    ${Qt5Core_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${Qt5WebKitWidgets_LIBRARIES}
    ${Qt5WebKit_LIBRARIES}
    marblewidget)
endif()

if(WIN32)
  target_link_libraries(
    marbleui
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
