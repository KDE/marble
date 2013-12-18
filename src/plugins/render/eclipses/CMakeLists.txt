PROJECT(EclipsesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib/astro
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/eclipses
 ${CMAKE_BINARY_DIR}/src/plugins/render/eclipses
 ${QT_INCLUDE_DIR}
)

if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( eclipses_SRCS
    EclipsesModel.cpp
    EclipsesItem.cpp
    EclipsesPlugin.cpp
    EclipsesBrowserDialog.cpp )

qt_wrap_ui( eclipses_SRCS
    EclipsesConfigDialog.ui
    EclipsesBrowserDialog.ui
    EclipsesReminderDialog.ui )

qt_add_resources( eclipses_SRCS eclipses.qrc )

marble_add_plugin( EclipsesPlugin ${eclipses_SRCS} )

target_link_libraries( EclipsesPlugin astro )
