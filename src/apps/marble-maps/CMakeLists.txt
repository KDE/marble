set(marble_SRCS main.cpp SearchBackend.cpp)
qt5_add_resources(marble_QRCS MarbleMaps.qrc)

if (ANDROID)
    set(TARGET Marble)
    add_library (${TARGET} SHARED ${marble_SRCS} ${marble_QRCS})
else()
    set(TARGET marble-maps)
    add_executable (${TARGET} ${marble_SRCS} ${marble_QRCS})
endif()

target_link_libraries (
      ${TARGET}
      ${Qt5Core_LIBRARIES}
      ${Qt5Xml_LIBRARIES}
      ${Qt5Widgets_LIBRARIES}
      ${Qt5PrintSupport_LIBRARIES}
      ${Qt5Network_LIBRARIES}
      ${Qt5WebKitWidgets_LIBRARIES}
      ${Qt5WebKit_LIBRARIES}
      ${Qt5Sql_LIBRARIES}
      ${Qt5Location_LIBRARIES}
      ${Qt5Positioning_LIBRARIES}
      ${MARBLEWIDGET}
      astro)

FILE(GLOB QML_FILES *.qml)
add_custom_target(marble-maps_resources ALL SOURCES ${QML_FILES} package/AndroidManifest.xml)

if (ANDROID)
    install(DIRECTORY "../../../data/android/" DESTINATION "${CMAKE_INSTALL_PREFIX}/res")
    set(ABSOLUTE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
    if(NOT IS_ABSOLUTE "${ABSOLUTE_INSTALL_PATH}")
        set(ABSOLUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}")
    endif()
    get_filename_component(ABSOLUTE_INSTALL_PATH "${ABSOLUTE_INSTALL_PATH}" ABSOLUTE)
    get_filename_component(QT_ANDROID_QT_ROOT "${Qt5Core_DIR}/../../.." ABSOLUTE)
    set(QT_ANDROID_SDK_ROOT $ENV{ANDROID_SDK})
    set(QT_ANDROID_NDK_ROOT $ENV{ANDROID_NDK})
    set(QT_ANDROID_APP_PATH "${ABSOLUTE_INSTALL_PATH}/libs/${ANDROID_ABI}/libMarble.so")
    set(QT_ANDROID_APP_EXTRA_LIBS "${ABSOLUTE_INSTALL_PATH}/lib/libmarblewidget-qt5.so,${ABSOLUTE_INSTALL_PATH}/lib/libastro.so")
    set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/package")
    set(QT_ANDROID_APP_PACKAGE_NAME "org.kde.marble.maps")
    set(QT_ANDROID_APP_NAME "Marble Maps")
    configure_file(package/deploy-marble-maps.json.in ${CMAKE_CURRENT_BINARY_DIR}/deploy-marble-maps.json @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/deploy-marble-maps.json DESTINATION "${CMAKE_INSTALL_PREFIX}/share")
    install(TARGETS ${TARGET} LIBRARY DESTINATION libs/${ANDROID_ABI})
else()
    install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
endif()
