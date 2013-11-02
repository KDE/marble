PROJECT( LocalOsmSearchPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
else()
  INCLUDE_DIRECTORIES(${Qt5Sql_INCLUDE_DIRS})
endif()

set( localOsmSearch_SRCS
LocalOsmSearchRunner.cpp
LocalOsmSearchPlugin.cpp
OsmPlacemark.cpp
OsmDatabase.cpp
DatabaseQuery.cpp
 )

marble_add_plugin( LocalOsmSearchPlugin ${localOsmSearch_SRCS} )
target_link_libraries( LocalOsmSearchPlugin ${QT_QTSQL_LIBRARY} ${Qt5Sql_LIBRARIES} )
