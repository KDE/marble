#############################################################

ADD_DEFINITIONS(-DTEST_DATA_DIR="${TEST_DATA_DIR}")
ADD_DEFINITIONS(-DMARBLE_SRC_DIR="${CMAKE_SOURCE_DIR}")

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

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

#############################################################
# includes

#############################################################
# Make the current test source directory available in #define TESTSRCDIR
# use this for accessing TESTSRCDIR/data
#############################################################

add_definitions(-DTESTSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")

#############################################################

#############################################################
# Configure the test scripts and put them in the project root
# directory. 
#############################################################

#make sure that the cmake and ctest stuff is available
if(CMAKE_COMMAND AND CMAKE_CTEST_COMMAND)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CTestNightlyScript.cmake.in"
    "${PROJECT_BINARY_DIR}/CTestNightlyScript.cmake")

else( CMAKE_COMMAND AND CMAKE_CTEST_COMMAND)

message( STATUS "Both the CTest and CMake command are needed to create test scripts") 
message( STATUS "Test Scripts have not been created")

endif( CMAKE_COMMAND AND CMAKE_CTEST_COMMAND)


############################
# Drop in New Tests
############################
marble_add_test( MarbleWidgetSpeedTest)
add_definitions(-DDGML_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../data/maps/earth")
marble_add_test( TestGeoSceneWriter)

marble_add_test( LocaleTest)               # Check MarbleLocale functionality
marble_add_test( QuaternionTest)           # Check Quaternion arithmetic
marble_add_test( TileIdTest)               # Check TileId arithmetic
marble_add_test( ViewportParamsTest)
marble_add_test( PluginManagerTest)        # Check plugin loading
marble_add_test( MarbleRunnerManagerTest)  # Check RunnerManager signals
marble_add_test( BookmarkManagerTest)
marble_add_test( PlacemarkPositionProviderPluginTest)
marble_add_test( PositionTrackingTest)
marble_add_test( MercatorProjectionTest)   # Check Screen coordinates
marble_add_test( GnomonicProjectionTest)
marble_add_test( StereographicProjectionTest)
marble_add_test( MarbleMapTest)            # Check map theme and centering
marble_add_test( MarbleWidgetTest)         # Check map theme, mouse move, repaint and multiple widgets
marble_add_test( MapViewWidgetTest)        # Check mapview signals
marble_add_test( TestGeoPainter)           # no tests!
marble_add_test( GeoUriParserTest)
marble_add_test( BillboardGraphicsItemTest)
marble_add_test( ScreenGraphicsItemTest)
marble_add_test( FrameGraphicsItemTest)
marble_add_test( RenderPluginTest)
marble_add_test( AbstractDataPluginModelTest)
marble_add_test( AbstractDataPluginTest)
marble_add_test( AbstractFloatItemTest)
marble_add_test( RenderPluginModelTest)
marble_add_test( GeoDataTreeModelTest)
marble_add_test( RouteRequestTest)

## GeoData Classes tests
marble_add_test( TestCamera)
marble_add_test( TestNetworkLink)
marble_add_test( TestLatLonQuad)
marble_add_test( TestGeoData)                  # Check parent, nodetype
marble_add_test( TestGeoDataCoordinates)       # Check coordinates specifics
marble_add_test( TestGeoDataLatLonAltBox)      # Check boxen specifics
marble_add_test( TestGeoDataGeometry)          # Check geometry specifics
marble_add_test( TestGeoDataTrack)             # Check track specifics
marble_add_test( TestGxTimeSpan)
marble_add_test( TestGxTimeStamp)
marble_add_test( TestBalloonStyle)             # Check BalloonStyle
marble_add_test( TestListStyle)                # Check ListStyle
marble_add_test( TestTour)
marble_add_test( TestGroundOverlay)     # Check GroundOverlay specifics
marble_add_test( TestModel)
marble_add_test( TestTimeStamp)
marble_add_test( TestTimeSpan)
marble_add_test( TestEquality)
marble_add_test( TestFeatureDetach)
marble_add_test( TestGeometryDetach)
marble_add_test( TestTileProjection)
marble_add_test( TestGeoDataBuilding)

qt_add_resources(TestGeoDataCopy_SRCS TestGeoDataCopy.qrc) # Check copy operations on CoW classes
marble_add_test( TestGeoDataCopy ${TestGeoDataCopy_SRCS})

add_definitions(-DCITIES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../data/placemarks/cityplacemarks.kml")
marble_add_test( TestGeoDataWriter)            # Check parsing, writing, reloading and comparing kml files
# marble_add_test( TestGeoDataPack)              # Check pack and unpack to file
