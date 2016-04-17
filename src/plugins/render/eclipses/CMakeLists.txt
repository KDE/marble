PROJECT(EclipsesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib/astro
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

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
