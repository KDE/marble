PROJECT( CycleStreetsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( cyclestreets_SRCS CycleStreetsRunner.cpp CycleStreetsPlugin.cpp )

set( cyclestreets_UI CycleStreetsConfigWidget.ui )

qt_wrap_ui( cyclestreets_SRCS ${cyclestreets_UI} )

marble_add_plugin( CycleStreetsPlugin ${cyclestreets_SRCS} )
