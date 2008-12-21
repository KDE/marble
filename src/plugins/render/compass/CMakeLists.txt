PROJECT(CompassFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/compass
 ${CMAKE_BINARY_DIR}/src/plugins/marble/compass
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( compass_SRCS CompassFloatItem.cpp )

marble_add_plugin( CompassFloatItem ${compass_SRCS} )