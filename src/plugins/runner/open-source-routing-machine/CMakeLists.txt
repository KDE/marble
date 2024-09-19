PROJECT( OSRMPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( osrm_SRCS OSRMRunner.cpp OSRMPlugin.cpp OSRMRunner.h OSRMPlugin.h)

marble_add_plugin( OSRMPlugin ${osrm_SRCS} )
