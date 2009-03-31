PROJECT( QNamNetworkPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/network/qnam
 ${CMAKE_BINARY_DIR}/src/plugins/network/qnam
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( qnam_SRCS QNamDownloadJob.cpp
               QNamNetworkPlugin.cpp )

marble_add_plugin( QNamNetworkPlugin ${qnam_SRCS} )
