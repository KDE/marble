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
# Make the current test source directory available in #define TESTSRCDIR
# use this for accessing TESTSRCDIR/data
#############################################################

add_definitions( -DTESTSRCDIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\" )

#############################################################

#############################################################
# Configure the test scripts and put them in the project root
# directory. 
#############################################################

#make sure that the cmake and ctest stuff is available
if( CMAKE_COMMAND AND CMAKE_CTEST_COMMAND )

configure_file( 
    "${CMAKE_CURRENT_SOURCE_DIR}/CTestNightlyScript.cmake.in"
    "${PROJECT_BINARY_DIR}/CTestNightlyScript.cmake" )

else( CMAKE_COMMAND AND CMAKE_CTEST_COMMAND )

message( STATUS "Both the CTest and CMake command are needed to create test scripts" ) 
message( STATUS "Test Scripts have not been created" )

endif( CMAKE_COMMAND AND CMAKE_CTEST_COMMAND )


#############################################################
# Add extra Libs for testing Plugins
#############################################################
#FIXME: Find a better way to do this!
#set(osmannotate_SRCS
#    ../src/plugins/render/osmannotate/TextAnnotation.cpp
#    ../src/plugins/render/osmannotate/PlacemarkTextAnnotation.cpp
#    ../src/plugins/render/osmannotate/GeoWidgetBubble.cpp
#    ../src/plugins/render/osmannotate/TmpGraphicsItem.cpp
#    ../src/plugins/render/osmannotate/TextEditor.cpp )

#add_library(OsmAnnotatePluginTestLib SHARED ${osmannotate_SRCS} )
#target_link_libraries( OsmAnnotatePluginTestLib
#                            ${QT_QTGUI_LIBRARY}
#                            ${QT_QTMAIN_LIBRARY}
#                            ${QT_QTCORE_LIBRARY}
#                            marblewidget )
#
#############################################################

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

#FIXME: this stuff should actually be done only once and in the main CMake File!
# include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/plugins/render/osmannotate )


#################################################
# dumps a dgml or a kml file without testing anything
# FIXME turn into automated test or remove
#################################################
add_executable(geodatatest GeoDataTest.cpp)
set_target_properties( geodatatest PROPERTIES COMPILE_FLAGS "-DKML_LAZY_IMP" )
target_link_libraries (geodatatest marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})


############################
# Drop in New Tests
############################
marble_add_test( MarbleWidgetSpeedTest )
add_definitions( -DDGML_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}/../data/maps/earth\\\"" )
marble_add_test( TestGeoSceneWriter )

marble_add_test( QuaternionTest )           # Check Quaternion arithmetic
marble_add_test( PluginManagerTest )        # Check plugin loading
marble_add_test( MarbleRunnerManagerTest )  # Check RunnerManager signals
marble_add_test( PlacemarkPositionProviderPluginTest )
marble_add_test( PositionTrackingTest )
marble_add_test( MercatorProjectionTest )   # Check Screen coordinates
marble_add_test( MarbleMapTest )            # Check map theme and centering
marble_add_test( MarbleWidgetTest )         # Check map theme, mouse move, repaint and multiple widgets
marble_add_test( MapViewWidgetTest )        # Check mapview signals
marble_add_test( TestGeoPainter )           # no tests!
marble_add_test( GeoPolygonTest )           # Loads an empty pnt file
#marble_add_test( TestOsmAnnotation )

## GeoData Classes tests
marble_add_test( TestGeoData )                  # Check parent, nodetype
marble_add_test( TestGeoDataCoordinates )       # Check coordinates specifics
marble_add_test( TestGeoDataLatLonAltBox )      # Check boxen specifics
marble_add_test( TestGeoDataGeometry )          # Check geometry specifics
marble_add_test( TestGeoDataTrack )             # Check track specifics

qt4_add_resources(TestGeoDataCopy_SRCS TestGeoDataCopy.qrc) # Check copy operations on CoW classes
marble_add_test( TestGeoDataCopy ${TestGeoDataCopy_SRCS} )

add_definitions( -DCITIES_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}/../data/placemarks/cityplacemarks.kml\\\"" )
marble_add_test( TestGeoDataWriter )            # Check parsing, writing, reloading and comparing kml files
marble_add_test( TestGeoDataPack )              # Check pack and unpack to file


