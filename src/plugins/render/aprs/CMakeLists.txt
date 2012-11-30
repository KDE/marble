PROJECT(AprsPlugin)

macro_optional_find_package(QextSerialPort)
if (QEXTSERIALPORT_FOUND)
  message( STATUS "Building APRS plugin with serial port input")
endif (QEXTSERIALPORT_FOUND)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

if (QEXTSERIALPORT_FOUND)
  include_directories (${QEXTSERIALPORT_INCLUDE_DIR})
  set (serial_src AprsTTY.cpp)
  list (APPEND AprsPlugin_LIBS ${QEXTSERIALPORT_LIBRARIES})
  set (HAVE_QEXTSERIALPORT TRUE)
endif ()

set( aprs_SRCS AprsPlugin.cpp
               AprsObject.cpp
	       AprsGatherer.cpp
	       GeoAprsCoordinates.cpp

	       AprsSource.cpp
	       AprsTCPIP.cpp
	       AprsFile.cpp

	       ${serial_src} )

set( aprs_UI AprsConfigWidget.ui )
qt4_wrap_ui( aprs_SRCS ${aprs_UI} )

if (QTONLY)
qt4_automoc( ${aprs_SRCS} )
endif (QTONLY)

marble_add_plugin( AprsPlugin ${aprs_SRCS} )
#target_link_libraries( Aprs ${QT_QTWEBKIT_LIBRARY} )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/aprsconfig.h.in
	       ${CMAKE_CURRENT_BINARY_DIR}/aprsconfig.h)
