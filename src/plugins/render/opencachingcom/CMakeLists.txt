PROJECT( OpenCachingComPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_BINARY_DIR}/src/plugins/render/opencachingcom
 ${QT_INCLUDE_DIR}
)

SET( my_SRCS
  OpenCachingComPlugin.cpp
  OpenCachingComModel.cpp
  OpenCachingComItem.cpp
 )
# SET( opencaching_UI OpenCachingConfigWidget.ui )
SET( cachedetailsdialog_UI CacheDetailsDialog.ui )

# QT_WRAP_UI(my_SRCS ${opencaching_UI})
QT_WRAP_UI(my_SRCS ${cachedetailsdialog_UI})

qt_add_resources(my_SRCS opencachingcom.qrc)

MARBLE_ADD_PLUGIN( OpenCachingComPlugin ${my_SRCS} )

