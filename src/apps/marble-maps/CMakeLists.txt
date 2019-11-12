set(marble_SRCS main.cpp MarbleMaps.cpp TextToSpeechClient.cpp)
qt5_add_resources(marble_QRCS MarbleMaps.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(marble-maps ${marble_SRCS} ${marble_QRCS})

if (ANDROID)
    find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED
        COMPONENTS
            AndroidExtras
            QuickControls2
    )

    # while not directly linked, need to be listed here to have deployqt pick up those libs
    target_link_libraries (
        marble-maps
        marblewidget
        astro
        Qt5::Concurrent
        Qt5::Xml
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Network
        Qt5::Sql
        Qt5::Svg
        Qt5::Positioning
        Qt5::AndroidExtras
        Qt5::Multimedia
        Qt5::QuickControls2
    )

    if (TARGET Qt5::WebEngineWidgets)
        target_link_libraries(${TARGET} Qt5::WebEngineWidgets Qt5::WebEngine)
    endif()
endif()

target_link_libraries (
    marble-maps
    marbledeclarative
)

FILE(GLOB QML_FILES *.qml)
add_custom_target(marble-maps_resources ALL SOURCES ${QML_FILES} package/AndroidManifest.xml)
