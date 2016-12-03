set(marble_SRCS main.cpp MarbleMaps.cpp TextToSpeechClient.cpp)
qt5_add_resources(marble_QRCS MarbleMaps.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    set(TARGET Marble)

    find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED
        COMPONENTS
            AndroidExtras
            QuickControls2
    )

    add_library (${TARGET} SHARED ${marble_SRCS} ${marble_QRCS})

    # while not directly linked, need to be listed here to have deployqt pick up those libs
    target_link_libraries (
        ${TARGET}
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
else()
    set(TARGET marble-maps)
    add_executable (${TARGET} ${marble_SRCS} ${marble_QRCS})
endif()

target_link_libraries (
    ${TARGET}
    marbledeclarative
)

FILE(GLOB QML_FILES *.qml)
add_custom_target(marble-maps_resources ALL SOURCES ${QML_FILES} package/AndroidManifest.xml)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    install(DIRECTORY "../../../data/android/" DESTINATION "${CMAKE_INSTALL_PREFIX}/res")
    set(ABSOLUTE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
    if(NOT IS_ABSOLUTE "${ABSOLUTE_INSTALL_PATH}")
        set(ABSOLUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}")
    endif()
    get_filename_component(ABSOLUTE_INSTALL_PATH "${ABSOLUTE_INSTALL_PATH}" ABSOLUTE)
    get_filename_component(QT_ANDROID_QT_ROOT "${Qt5Core_DIR}/../../.." ABSOLUTE)
    set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
    set(ANDROID_NDK_ROOT $ENV{ANDROID_NDK_ROOT})
    set(QT_ANDROID_APP_PATH "${ABSOLUTE_INSTALL_PATH}/libs/${ANDROID_ABI}/libMarble.so")
    set(QT_ANDROID_APP_EXTRA_LIBS "${ABSOLUTE_INSTALL_PATH}/lib/libastro.so,${ABSOLUTE_INSTALL_PATH}/lib/libmarblewidget-qt5.so,${ABSOLUTE_INSTALL_PATH}/lib/libmarbledeclarative.so")
    set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/package")
    set(QT_ANDROID_APP_PACKAGE_NAME "org.kde.marble.maps")
    set(QT_ANDROID_APP_NAME "Marble Maps")
    configure_file(package/deploy-marble-maps.json.in ${CMAKE_CURRENT_BINARY_DIR}/deploy-marble-maps.json @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/deploy-marble-maps.json DESTINATION "${CMAKE_INSTALL_PREFIX}/share")
    install(TARGETS ${TARGET} LIBRARY DESTINATION libs/${ANDROID_ABI})
else()
    if(MARBLE_INSTALL_ANDROID_APPS)
        if(APPS_INSTALL_DIR)
            install(PROGRAMS org.kde.marble.maps.desktop DESTINATION ${APPS_INSTALL_DIR})
        endif(APPS_INSTALL_DIR)
        install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
    endif()
endif()
