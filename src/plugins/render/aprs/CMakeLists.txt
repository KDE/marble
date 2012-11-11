PROJECT(AprsPlugin)

INCLUDE (CheckFunctionExists)
CHECK_FUNCTION_EXISTS(cfmakeraw HAVE_CFMAKERAW)

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/filio.h HAVE_SYS_FILIO_H)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

if (WIN32)
  set (serial_src win_qextserialport.cpp)
  ADD_DEFINITIONS(-DQEXTSERIALPORT_LIB)
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
