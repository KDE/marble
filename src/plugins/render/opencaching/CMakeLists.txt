PROJECT( OpenCachingPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/opencaching
 ${CMAKE_BINARY_DIR}/src/plugins/render/opencaching
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

SET( my_SRCS
  OpenCachingPlugin.cpp
  OpenCachingModel.cpp
  OpenCachingItem.cpp
  OpenCachingCache.cpp
  OpenCachingCacheDescription.cpp
  OpenCachingCacheLog.cpp
  OpenCachingCacheLogEntry.cpp
 )
SET( opencaching_UI OpenCachingConfigWidget.ui )
SET( opencachingcache_UI OpenCachingCacheDialog.ui )

QT4_WRAP_UI(my_SRCS ${opencaching_UI})
QT4_WRAP_UI(my_SRCS ${opencachingcache_UI})
QT4_AUTOMOC( ${my_SRCS} )

MARBLE_ADD_PLUGIN( OpenCachingPlugin ${my_SRCS} )
