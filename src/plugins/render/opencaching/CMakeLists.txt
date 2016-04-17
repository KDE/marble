PROJECT( OpenCachingPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

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

QT_WRAP_UI(my_SRCS ${opencaching_UI})
QT_WRAP_UI(my_SRCS ${opencachingcache_UI})

MARBLE_ADD_PLUGIN( OpenCachingPlugin ${my_SRCS} )
