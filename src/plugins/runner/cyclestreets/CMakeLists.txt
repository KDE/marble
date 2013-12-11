PROJECT( CycleStreetsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
else()
INCLUDE_DIRECTORIES(
  ${Qt5WebKitWidgets_INCLUDE_DIRS}
  ${Qt5Xml_INCLUDE_DIRS}
)
endif()

set( cyclestreets_SRCS CycleStreetsRunner.cpp CycleStreetsPlugin.cpp )

marble_add_plugin( CycleStreetsPlugin ${cyclestreets_SRCS} )