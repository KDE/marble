PROJECT( LocalOsmSearchPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( localOsmSearch_SRCS
LocalOsmSearchRunner.cpp
LocalOsmSearchPlugin.cpp
OsmPlacemark.cpp
OsmDatabase.cpp
DatabaseQuery.cpp
 )

marble_add_plugin( LocalOsmSearchPlugin ${localOsmSearch_SRCS} )
target_link_libraries( LocalOsmSearchPlugin Qt5::Sql )
