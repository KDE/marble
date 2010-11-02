PROJECT(RoutingPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/routing
 ${CMAKE_BINARY_DIR}/src/plugins/render/routing
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( routing_SRCS RoutingPlugin.cpp )		 
set( routing_UI RoutingPlugin.ui )
qt4_wrap_ui( routing_SRCS ${routing_UI} )

marble_add_plugin( RoutingPlugin ${routing_SRCS} )
