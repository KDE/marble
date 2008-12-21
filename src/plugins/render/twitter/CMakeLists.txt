PROJECT(twitter)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/twitter
 ${CMAKE_BINARY_DIR}/src/plugins/marble/twitter
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( twitter_SRCS twitterPlugin.cpp jsonparser.cpp )

marble_add_plugin( twitter ${twitter_SRCS} )