PROJECT( FoursquarePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/foursquare
 ${CMAKE_BINARY_DIR}/src/plugins/render/foursquare
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( my_SRCS
  FoursquarePlugin.cpp
  FoursquareModel.cpp
  FoursquareItem.cpp
 )

qt4_automoc( ${my_SRCS} )

marble_add_plugin( FoursquarePlugin ${my_SRCS} )

