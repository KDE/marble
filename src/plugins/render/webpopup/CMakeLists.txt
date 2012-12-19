PROJECT(WebPopupFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( webpopup_SRCS WebPopupFloatItem.cpp PopupItem.cpp )
qt4_add_resources( webpopup_SRCS webpopup.qrc )


marble_add_plugin( WebPopupFloatItem ${webpopup_SRCS} )
