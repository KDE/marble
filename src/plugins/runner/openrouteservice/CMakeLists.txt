PROJECT( OpenRouteServicePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/openrouteservice
 ${CMAKE_BINARY_DIR}/src/plugins/runner/openrouteservice
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( openrouteservice_SRCS OpenRouteServiceRunner.cpp OpenRouteServicePlugin.cpp )

set( openrouteservice_UI OpenRouteServiceConfigWidget.ui )

qt4_wrap_ui( openrouteservice_SRCS ${openrouteservice_UI} )

marble_add_plugin( OpenRouteServicePlugin ${openrouteservice_SRCS} )
