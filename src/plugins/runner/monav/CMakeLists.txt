PROJECT( MonavPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
else()
  INCLUDE_DIRECTORIES(${Qt5Xml_INCLUDE_DIRS})
endif()

set( monav_SRCS
  MonavRunner.cpp
  MonavPlugin.cpp
  MonavMap.cpp
  MonavMapsModel.cpp
  MonavConfigWidget.cpp )

set( monav_UI MonavConfigWidget.ui )
qt_wrap_ui( monav_SRCS ${monav_UI} )
qt_add_resources( monav_SRCS monav.qrc )

marble_add_plugin( MonavPlugin ${monav_SRCS} )
