PROJECT(EclipsesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/eclipses
 ${CMAKE_BINARY_DIR}/src/plugins/render/eclipses
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( ecl_SRCS
    ecl/astrolib.cpp
    ecl/attlib.cpp
    ecl/eclsolar.cpp )

set( eclipses_SRCS
    EclipsesModel.cpp
    EclipsesItem.cpp
    EclipsesPlugin.cpp )

qt4_wrap_ui( eclipses_SRCS
    EclipsesConfigDialog.ui
    EclipsesListDialog.ui
    EclipsesReminderDialog.ui )

qt4_add_resources( eclipses_SRCS eclipses.qrc )

marble_add_plugin( EclipsesPlugin ${eclipses_SRCS} ${ecl_SRCS} )

