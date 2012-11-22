PROJECT(twitter)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( twitter_SRCS twitterPlugin.cpp jsonparser.cpp )

marble_add_plugin( twitter ${twitter_SRCS} )
target_link_libraries( twitter ${QT_QTSCRIPT_LIBRARY} )
