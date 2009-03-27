PROJECT( QHttpNetworkPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/network/qhttp
 ${CMAKE_BINARY_DIR}/src/plugins/network/qhttp
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( qhttp_SRCS QHttpNetworkPlugin.cpp )

marble_add_plugin( QHttpNetworkPlugin ${qhttp_SRCS} )
