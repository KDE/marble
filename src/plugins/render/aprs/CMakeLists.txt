PROJECT(AprsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/aprs
 ${CMAKE_BINARY_DIR}/src/plugins/render/aprs
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

if (WIN32)
  set (serial_src win_qextserialport.cpp)
else (WIN32)
  set (serial_src posix_qextserialport.cpp)
endif (WIN32)

set( aprs_SRCS AprsPlugin.cpp
               AprsObject.cpp
	       AprsGatherer.cpp
	       GeoAprsCoordinates.cpp

	       AprsSource.cpp
	       AprsTTY.cpp
	       AprsTCPIP.cpp
	       AprsFile.cpp

               qextserialport.cpp
	       posix_qextserialport.cpp )

set( aprs_UI AprsConfigWidget.ui )
qt4_wrap_ui( aprs_SRCS ${aprs_UI} )

marble_add_plugin( AprsPlugin ${aprs_SRCS} )
#target_link_libraries( Aprs ${QT_QTWEBKIT_LIBRARY} )
PROJECT(AprsPlugin)
