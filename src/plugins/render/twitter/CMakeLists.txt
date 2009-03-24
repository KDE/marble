PROJECT(twitter)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/twitter
 ${CMAKE_BINARY_DIR}/src/plugins/render/twitter
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( twitter_SRCS twitterPlugin.cpp jsonparser.cpp )

marble_add_plugin( twitter ${twitter_SRCS} )
target_link_libraries( twitter ${QT_QTSCRIPT_LIBRARY} )
