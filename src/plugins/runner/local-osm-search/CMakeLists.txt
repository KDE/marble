PROJECT( LocalOsmSearchPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(localOsmSearch_SRCS
LocalOsmSearchRunner.cpp
LocalOsmSearchPlugin.cpp
OsmPlacemark.cpp
OsmDatabase.cpp
DatabaseQuery.cpp
LocalOsmSearchRunner.h
LocalOsmSearchPlugin.h
OsmPlacemark.h
OsmDatabase.h
DatabaseQuery.h
)

marble_add_plugin( LocalOsmSearchPlugin ${localOsmSearch_SRCS})
target_link_libraries(LocalOsmSearchPlugin Qt6::Sql)
