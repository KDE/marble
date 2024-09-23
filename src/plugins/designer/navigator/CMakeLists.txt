PROJECT(MarbleNavigatorPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${Qt6UiPlugin_INCLUDE_DIRS}
 ${Qt6Designer_INCLUDE_DIRS}
)

set( marblenavigator_SRCS MarbleNavigatorPlugin.cpp MarbleNavigatorPlugin.h)

marble_add_designer_plugin( MarbleNavigatorPlugin ${marblenavigator_SRCS} )

