set(marble_SRCS main.cpp)
marble_qt4_automoc(${marble_SRCS})

add_library (Marble SHARED ${marble_SRCS})
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
 
include(../../../qt-android-cmake/AddQtAndroidApk.cmake)
add_qt_android_apk(Marble_apk Marble DEPENDS "../../../../build/src/lib/marble/libmarblewidget-qt5.so" "../../../../build/src/lib/astro/libastro.so" NAME "Marble Maps" PACKAGE_NAME "org.kde.marble.maps")