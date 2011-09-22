PROJECT( GpxPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/handlers
 ${CMAKE_BINARY_DIR}/src/plugins/runner/gpx
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( gpx_handlers_SRCS
        handlers/GPXElementDictionary.cpp
        handlers/GPXgpxTagHandler.cpp
        handlers/GPXnameTagHandler.cpp
        handlers/GPXtrkTagHandler.cpp
        handlers/GPXtrkptTagHandler.cpp
        handlers/GPXtrksegTagHandler.cpp
        handlers/GPXwptTagHandler.cpp
        handlers/GPXrteTagHandler.cpp
        handlers/GPXrteptTagHandler.cpp
   )

set( gpx_SRCS GpxParser.cpp GpxPlugin.cpp GpxRunner.cpp )

marble_add_plugin( GpxPlugin ${gpx_SRCS}  ${gpx_handlers_SRCS} )
