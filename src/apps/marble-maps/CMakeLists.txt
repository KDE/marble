set(marble_SRCS main.cpp)
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
      ${MARBLEWIDGET}
      astro)

file(COPY "../../../data/android/drawable-ldpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble-maps/res")
file(COPY "../../../data/android/drawable-mdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble-maps/res")
file(COPY "../../../data/android/drawable-hdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble-maps/res")
file(COPY "../../../data/android/drawable-xhdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble-maps/res")
file(COPY "../../../data/android/drawable-xxhdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble-maps/res")
file(COPY "../../../data/android/drawable-xxxhdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble-maps/res")

add_custom_target(marble-maps_resources ALL SOURCES MainScreen.qml package/AndroidManifest.xml)

if (ANDROID)
    include("$ENV{Qt5_android_cmake}/AddQtAndroidApk.cmake")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Marble.keystore")
        FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/package" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/package/project.properties" "key.store=${CMAKE_CURRENT_SOURCE_DIR}/Marble.keystore\nkey.alias=Marble\n")
        set(ANDROID_APK_OPTIONS KEYSTORE ${CMAKE_CURRENT_SOURCE_DIR}/Marble.keystore Marble PACKAGE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/package")
    else()
        set(ANDROID_APK_OPTIONS PACKAGE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/package")
    endif()
    add_qt_android_apk(Marble_apk Marble
        PACKAGE_NAME org.kde.marble.maps
        ${ANDROID_APK_OPTIONS}
        DEPENDS "${PROJECT_BINARY_DIR}/lib/libmarblewidget-qt5.so" "${PROJECT_BINARY_DIR}/lib/libastro.so"
    )

    # Move target from default build to install phase so we can create everything in one go
    set_target_properties(Marble_apk PROPERTIES EXCLUDE_FROM_ALL TRUE)
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target Marble_apk)")
else()
    install (TARGETS ${TARGET} RUNTIME DESTINATION bin )
endif()
