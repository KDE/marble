if(ANDROID)
    set(marble_SRCS main.cpp)
    marble_qt4_automoc(${marble_SRCS})

    add_library (marble_android SHARED ${marble_SRCS})
    set_target_properties(marble_android PROPERTIES VERSION 0.0.1)

    target_link_libraries (
      marble_android
      ${Qt5Core_LIBRARIES}
      ${Qt5Xml_LIBRARIES}
      ${Qt5Widgets_LIBRARIES}
      ${Qt5PrintSupport_LIBRARIES}
      ${Qt5Network_LIBRARIES}
      ${Qt5WebKitWidgets_LIBRARIES}
      ${Qt5WebKit_LIBRARIES}
      ${MARBLEWIDGET}
      astro)
    
    include(../../../qt-android-cmake/AddQtAndroidApk.cmake)
    add_qt_android_apk(marble_android_apk marble_android DEPENDS "../../../../build/src/lib/marble/libmarblewidget-qt5.so" "../../../../build/src/lib/astro/libastro.so")    
endif(ANDROID)

