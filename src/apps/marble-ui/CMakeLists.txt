set(marble_SRCS ControlView.cpp MarbleTest.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.qrc)
endif(MSVC)

qt_add_resources(marble_SRCS marble.qrc)

qt_wrap_ui(marble_SRCS ${marble_UI})

add_library (marbleui STATIC ${marble_SRCS})

# TODO: use PUBLIC/PRIVATE everywhere instead of LINK_PUBLIC/LINK_PRIVATE
# once SailfishOS has cmake >= 2.8.12

target_link_libraries (
    marbleui
    LINK_PUBLIC
        marblewidget
        Qt5::Widgets
        Qt5::PrintSupport
    LINK_PRIVATE
        Qt5::Network
)
if (NOT MARBLE_NO_DBUS)
    target_link_libraries( marbleui
        LINK_PRIVATE
            Qt5::DBus
    )
endif ()

if(WIN32)
  # TODO: PUBLIC or can be PRIVATE?
  target_link_libraries(
    marbleui
    LINK_PUBLIC
        ws2_32
        imm32
        winmm
    )
endif(WIN32)

## this works for win32 only because FindQt does not handle this correct
if(STATIC_BUILD)
    # TODO: PUBLIC or can be PRIVATE?
    target_link_libraries(marble LINK_PUBLIC ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble LINK_PUBLIC ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)
