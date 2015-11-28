PROJECT( OpenLocationCodeSearchPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4BUILD )
  INCLUDE(${QT_USE_FILE})
else()
  INCLUDE_DIRECTORIES(${Qt5Sql_INCLUDE_DIRS})
endif()

set( openLocationCodeSearch_SRCS
OpenLocationCodeSearchPlugin.cpp
OpenLocationCodeSearchRunner.cpp
 )

marble_add_plugin( OpenLocationCodeSearchPlugin ${openLocationCodeSearch_SRCS} )
