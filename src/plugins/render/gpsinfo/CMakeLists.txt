PROJECT( GpsInfo )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( gpsinfo_SRCS GpsInfo.cpp )
set( gpsinfo_UI GpsInfoPlugin.ui )

qt4_wrap_ui(gpsinfo_SRCS ${gpsinfo_UI} )
marble_add_plugin( GpsInfo ${gpsinfo_SRCS} )
