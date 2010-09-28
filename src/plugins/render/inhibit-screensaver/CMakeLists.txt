PROJECT(InhibitScreensaverPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/inhibit-screensaver
 ${CMAKE_BINARY_DIR}/src/plugins/render/inhibit-screensaver
 ${QT_INCLUDE_DIR}
 ${QT_QTNETWORK_INCLUDE_DIR}
 ${QtSystemInfo_INCLUDE_DIRS}
)
INCLUDE(${QT_USE_FILE})

set( inhibit-screensaver_SRCS InhibitScreensaverPlugin.cpp )		 
set( InhibitScreensaverPlugin_LIBS ${QtSystemInfo_LIBRARIES} )

marble_add_plugin( InhibitScreensaverPlugin ${inhibit-screensaver_SRCS} )
