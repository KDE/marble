set(marble_SRCS ControlView.cpp MarbleTest.cpp ControlView.h MarbleTest.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.qrc)
endif(MSVC)

qt_add_resources(marble_SRCS marble.qrc)

qt_wrap_ui(marble_SRCS ${marble_UI})

add_library (marbleui STATIC ${marble_SRCS})

target_link_libraries (
    marbleui
    PUBLIC
        marblewidget
        Qt6::Widgets
        Qt6::PrintSupport
    PRIVATE
        Qt6::Network
)
if (NOT MARBLE_NO_DBUS)
    target_link_libraries( marbleui
        PRIVATE
            Qt6::DBus
    )
endif ()

if(WIN32)
  # TODO: PUBLIC or can be PRIVATE?
  target_link_libraries(
    marbleui
    PUBLIC
        ws2_32
        imm32
        winmm
    )
endif(WIN32)

## this works for win32 only because FindQt does not handle this correct
if(STATIC_BUILD AND WIN32)
    # TODO: PUBLIC or can be PRIVATE?
    target_link_libraries(marbleui PUBLIC ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marbleui PUBLIC ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif()
