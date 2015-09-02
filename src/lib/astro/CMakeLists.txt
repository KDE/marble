PROJECT(astro)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

SET(GENERIC_LIB_VERSION "0.17.20")
SET(GENERIC_LIB_SOVERSION "1")

ADD_DEFINITIONS(-O3)

SET(astrolib_SRCS 
  astrolib.cpp
  astr2lib.cpp
  attlib.cpp
  eclsolar.cpp
  planetarySats.cpp
  solarsystem.cpp
)

ADD_LIBRARY(astro SHARED ${astrolib_SRCS})

if (APPLE)
  #defined in top level makefile
  TARGET_LINK_LIBRARIES(astro ${MAC_EXTRA_LIBS})
endif (APPLE)

if (CMAKE_SYSTEM_NAME MATCHES "SunOS")
  TARGET_LINK_LIBRARIES(astro m)
endif (CMAKE_SYSTEM_NAME MATCHES "SunOS")

if(WIN32)
  TARGET_LINK_LIBRARIES(astro ws2_32 imm32 winmm)
endif(WIN32)


SET_TARGET_PROPERTIES(astro PROPERTIES
  VERSION ${GENERIC_LIB_VERSION}
  SOVERSION ${GENERIC_LIB_SOVERSION}
  DEFINE_SYMBOL MAKE_ASTRO_LIB            # needed for astrolib_export.h
  )

# choose the correct target install library path
if(WIN32)
  INSTALL(TARGETS astro RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
elseif(APPLE)
  INSTALL(TARGETS astro LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)
else()
  INSTALL(TARGETS astro LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

########### install files ###############

 INSTALL(FILES
    astrolib.h
    astr2lib.h
    attlib.h
    eclsolar.h
    planetarySats.h
    solarsystem.h
    astrolib_export.h

    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/astro
)
