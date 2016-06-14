PROJECT(MarbleNavigatorPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${Qt5UiPlugin_INCLUDE_DIRS}
 ${Qt5Designer_INCLUDE_DIRS}
)

set( marblenavigator_SRCS MarbleNavigatorPlugin.cpp )

marble_add_designer_plugin( MarbleNavigatorPlugin ${marblenavigator_SRCS} )

