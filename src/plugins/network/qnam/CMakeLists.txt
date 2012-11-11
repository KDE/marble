PROJECT( QNamNetworkPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( qnam_SRCS QNamDownloadJob.cpp
               QNamNetworkPlugin.cpp )

marble_add_plugin( QNamNetworkPlugin ${qnam_SRCS} )
