PROJECT(MarbleNavigatorPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblenavigator_SRCS MarbleNavigatorPlugin.cpp )

marble_add_designer_plugin( MarbleNavigatorPlugin ${marblenavigator_SRCS} )