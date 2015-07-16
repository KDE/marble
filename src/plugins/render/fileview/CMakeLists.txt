INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( fileview_SRCS FileViewFloatItem.cpp )
#set( fileview_UI fileview.ui )

#qt4_wrap_ui( fileview_SRCS ${fileview_UI} )

marble_add_plugin( FileViewFloatItem ${fileview_SRCS} )
