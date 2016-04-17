PROJECT( GpsInfo )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( gpsinfo_SRCS GpsInfo.cpp )
set( gpsinfo_UI GpsInfoPlugin.ui )

qt_wrap_ui(gpsinfo_SRCS ${gpsinfo_UI} )
marble_add_plugin( GpsInfo ${gpsinfo_SRCS} )
