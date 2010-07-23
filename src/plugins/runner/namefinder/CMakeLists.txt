PROJECT( NameFinderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/namefinder
 ${CMAKE_BINARY_DIR}/src/plugins/runner/namefinder
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( namefinder_SRCS OnfRunner.cpp NameFinderPlugin.cpp )

marble_add_plugin( NameFinderPlugin ${namefinder_SRCS} )
