#############################################################

ADD_DEFINITIONS(-DTEST_DATA_DIR="\\"${TEST_DATA_DIR}\\"")

#############################################################
# libraries

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

# Since the tests are not actually installed, but rather
# run directly from the build/src/tests dir we need to
# ensure the marble libs can be found.
IF (APPLE)
  # For Mac OS X, the executable must be at the root of the bundle's executable folder
  SET (CMAKE_INSTALL_NAME_DIR @executable_path/../lib)
ENDIF (APPLE)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

#############################################################

# MarbleWidget test
marble_add_test( MarbleWidgetSpeedTest )

# GeoPolygon test
if (QTONLY)
  # ce: don't know why this is needed here - on win32 
  # 'O2' is activated by default in release mode
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} -O3)
  endif(NOT WIN32)

  include_directories(
    ${QT_INCLUDE_DIR}
  )
else (QTONLY)
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -O3) 
  endif(NOT WIN32)

  include_directories(
    ${KDE4_INCLUDES}
  )

endif (QTONLY)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib/geodata )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib/geodata/data )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib/geodata/parser )
marble_add_test( GeoPolygonTest )
