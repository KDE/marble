PROJECT( FoursquarePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

set( my_SRCS
  FoursquarePlugin.cpp
  FoursquareModel.cpp
  FoursquareItem.cpp
 )

marble_add_plugin( FoursquarePlugin ${my_SRCS} )

