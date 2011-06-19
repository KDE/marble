PROJECT( AltitudeProfile )

set(KDE_MIN_VERSION "4.4.0")
find_package(KDE4 4.4.0 REQUIRED)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/altitude-profile
 ${CMAKE_BINARY_DIR}/src/plugins/render/altitude-profile
 ${QT_INCLUDE_DIR}
 ${KDE4_INCLUDES}
)
INCLUDE(${QT_USE_FILE})

set( altitudeprofile_SRCS AltitudeProfile.cpp PlotWidget.cpp )

marble_add_plugin( AltitudeProfile ${altitudeprofile_SRCS} )
target_link_libraries( AltitudeProfile
        ${KDE4_KDEUI_LIBS})
