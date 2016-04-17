PROJECT( TravelingSalesmanPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( travelingsalesman_SRCS TravelingSalesmanRunner.cpp TravelingSalesmanPlugin.cpp )

marble_add_plugin( TravelingSalesmanPlugin ${travelingsalesman_SRCS} )
