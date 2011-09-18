PROJECT( Speedometer )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( speedometer_SRCS Speedometer.cpp )
set( speedometer_UI Speedometer.ui )

qt4_wrap_ui(speedometer_SRCS ${speedometer_UI} )
marble_add_plugin( Speedometer ${speedometer_SRCS} )
