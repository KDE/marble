PROJECT( OpenCachingComPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/opencachingcom
 ${CMAKE_BINARY_DIR}/src/plugins/render/opencachingcom
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

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

MARBLE_QT4_AUTOMOC( ${my_SRCS} )

MARBLE_ADD_PLUGIN( OpenCachingComPlugin ${my_SRCS} )

