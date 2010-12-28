OPTION(BUILD_INHIBIT_SCREENSAVER_PLUGIN "Build the Maemo specific inhibit-screensaver plugin")

IF( BUILD_INHIBIT_SCREENSAVER_PLUGIN )
  PROJECT(InhibitScreensaverPlugin)

  INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/inhibit-screensaver
   ${CMAKE_BINARY_DIR}/src/plugins/render/inhibit-screensaver
   ${QT_INCLUDE_DIR}
   ${QT_QTNETWORK_INCLUDE_DIR}
  )
  INCLUDE(${QT_USE_FILE})

  set( inhibit-screensaver_SRCS InhibitScreensaverPlugin.cpp )		 

  marble_add_plugin( InhibitScreensaverPlugin ${inhibit-screensaver_SRCS} )
ENDIF()
