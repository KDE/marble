INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( fileview_SRCS FileViewFloatItem.cpp )
#set( fileview_UI fileview.ui )

marble_add_plugin( FileViewFloatItem ${fileview_SRCS} )
