#####################################################
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
# binary make output dirs for uics and mocs
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/../src
  ${CMAKE_CURRENT_BINARY_DIR}/../src/lib
# source dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib
# include dirs
  ${QT_INCLUDE_DIR}
  )
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

# Note for tests we should not include the moc of our
# qtests in the executable file list as the moc is
# directly included in the sources
# and should not be compiled twice. Trying to include
# them in will cause an error at build time 

# @TODO having to add ${QT_QTTEST_LIBRARY} is ugly - 
# it should not be needed since we add qttest up in 
# top level cmakelists. Work out why its not working 
# right on mac - timlinux
set (MARBLE_TEST_LIBS ${QT_LIBRARIES} ${QT_QTTEST_LIBRARY} marblewidget)

#############################################################
# To add a new test copy the entire blcck that follows
# and search and replace marblewidget with the name of
# your test. Do not include the workd 'test' in your search
# or replace strings. By convention, your test should be called
# <classname>test if it tests a specific class.
#

# MarbleWidget test
SET(marblewidgettest_SRCS marblewidgettest.cpp)
SET(marblewidgettest_MOC_CPPS marblewidgettest.cpp)
QT4_WRAP_CPP(marblewidgettest_MOC_SRCS ${marblewidgettest_MOC_CPPS})
ADD_CUSTOM_TARGET(marblewidgettestmoc ALL DEPENDS ${marblewidgettest_MOC_SRCS})
ADD_EXECUTABLE(marblewidgettest ${marblewidgettest_SRCS})
ADD_DEPENDENCIES(marblewidgettest marblewidgettestmoc)
TARGET_LINK_LIBRARIES(marblewidgettest ${MARBLE_TEST_LIBS})
INSTALL(TARGETS marblewidgettest RUNTIME DESTINATION ${MARBLE_TEST_DIR})
ADD_TEST(marblewidgettest ${MARBLE_TEST_DIR}/marblewidgettest)

# GeoPolygon test
SET(geopolygontest_SRCS geopolygontest.cpp)
SET(geopolygontest_MOC_CPPS geopolygontest.cpp)
QT4_WRAP_CPP(geopolygontest_MOC_SRCS ${geopolygontest_MOC_CPPS})
ADD_CUSTOM_TARGET(geopolygontestmoc ALL DEPENDS ${geopolygontest_MOC_SRCS})
ADD_EXECUTABLE(geopolygontest ${geopolygontest_SRCS})
ADD_DEPENDENCIES(geopolygontest geopolygontestmoc)
TARGET_LINK_LIBRARIES(geopolygontest ${MARBLE_TEST_LIBS})
INSTALL(TARGETS geopolygontest RUNTIME DESTINATION ${MARBLE_TEST_DIR})
ADD_TEST(geopolygontest ${MARBLE_TEST_DIR}/geopolygontest)


#############################################################
# Put the libs needed into the test dir
IF (WIN32)
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/lib/libmarblewidget.dll DESTINATION ${MARBLE_TEST_DIR})
ENDIF (WIN32)

IF (APPLE)
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/lib/libmarblewidget.5.0.0.dylib DESTINATION ${MARBLE_TEST_DIR}/lib/)
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/lib/libmarblewidget.5.dylib DESTINATION ${MARBLE_TEST_DIR}/lib/)
ENDIF (APPLE)
