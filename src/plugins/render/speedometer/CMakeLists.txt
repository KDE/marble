PROJECT( Speedometer )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( speedometer_SRCS Speedometer.cpp )
set( speedometer_UI Speedometer.ui )

qt_wrap_ui(speedometer_SRCS ${speedometer_UI} )
marble_add_plugin( Speedometer ${speedometer_SRCS} )
