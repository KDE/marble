PROJECT( FoursquarePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( my_SRCS
  FoursquarePlugin.cpp
  FoursquareModel.cpp
  FoursquareItem.cpp
 )

set( FoursquarePlugin_LIBS Qt5::Script Qt5::Svg )

marble_add_plugin( FoursquarePlugin ${my_SRCS} )

