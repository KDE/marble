PROJECT(AprsPlugin)

find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS SerialPort)
marble_set_package_properties( Qt5SerialPort PROPERTIES DESCRIPTION "Provides access to hardware and virtual serial ports." )
marble_set_package_properties( Qt5SerialPort PROPERTIES TYPE OPTIONAL PURPOSE "Reading from serial port in APRS plugin" )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

if (Qt5SerialPort_FOUND)
  include_directories (${Qt5SerialPort_INCLUDE_DIR})
  set (serial_src AprsTTY.cpp)
  list (APPEND AprsPlugin_LIBS ${Qt5SerialPort_LIBRARIES})
  set (HAVE_QTSERIALPORT TRUE)
endif ()

set( aprs_SRCS AprsPlugin.cpp
               AprsObject.cpp
	       AprsGatherer.cpp
	       GeoAprsCoordinates.cpp
	       ${CMAKE_CURRENT_BINARY_DIR}/AprsGatherer_mic_e.cpp

	       AprsSource.cpp
	       AprsTCPIP.cpp
	       AprsFile.cpp

	       ${serial_src} )

set( aprs_UI AprsConfigWidget.ui )
qt_wrap_ui( aprs_SRCS ${aprs_UI} )

add_custom_command(
  OUTPUT AprsGatherer_mic_e.cpp
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/AprsGatherGen.pl > AprsGatherer_mic_e.cpp
  MAIN_DEPENDENCY AprsGatherGen.pl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

marble_add_plugin( AprsPlugin ${aprs_SRCS} )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/aprsconfig.h.in
	       ${CMAKE_CURRENT_BINARY_DIR}/aprsconfig.h)
