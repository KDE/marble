set(marble_SRCS main.cpp QmlView.cpp)
qt5_add_resources(marble_QRCS MarbleMaps.qrc)

add_library (Marble SHARED ${marble_SRCS} ${marble_QRCS})
set_target_properties(Marble PROPERTIES VERSION 1.0)

target_link_libraries (
      Marble
      ${Qt5Core_LIBRARIES}
      ${Qt5Xml_LIBRARIES}
      ${Qt5Widgets_LIBRARIES}
      ${Qt5PrintSupport_LIBRARIES}
      ${Qt5Network_LIBRARIES}
      ${Qt5WebKitWidgets_LIBRARIES}
      ${Qt5WebKit_LIBRARIES}
      ${MARBLEWIDGET}
      astro)
    
file(COPY "../../../data" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/assets")
file(COPY "../../../data/android/drawable-ldpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/res")
file(COPY "../../../data/android/drawable-mdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/res")
file(COPY "../../../data/android/drawable-hdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/res")
file(COPY "../../../data/android/drawable-xhdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/res")
file(COPY "../../../data/android/drawable-xxhdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/res")
file(COPY "../../../data/android/drawable-xxxhdpi" DESTINATION "${CMAKE_BINARY_DIR}/src/apps/marble_maps/res")

include("$ENV{Qt5_android_cmake}/AddQtAndroidApk.cmake")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Marble.keystore")
    set(KEYSTORE_OPTIONS KEYSTORE ${CMAKE_CURRENT_SOURCE_DIR}/Marble.keystore Marble)
endif()
add_qt_android_apk(Marble_apk Marble
    PACKAGE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/package"
    PACKAGE_NAME org.kde.marble.maps
    ${KEYSTORE_OPTIONS}
    DEPENDS "${PROJECT_BINARY_DIR}/lib/libmarblewidget-qt5.so" "${PROJECT_BINARY_DIR}/lib/libastro.so"
)

# Move target from default build to install phase so we can create everything in one go
set_target_properties(Marble_apk PROPERTIES EXCLUDE_FROM_ALL TRUE)
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target Marble_apk)")
