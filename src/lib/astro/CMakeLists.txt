PROJECT(astro)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(ASTRO_LIB_VERSION "0.17.20")
set(ASTRO_LIB_SOVERSION "1")

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
generate_export_header(astro BASE_NAME astrolib)
target_include_directories(astro PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

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

set_target_properties(astro PROPERTIES
    EXPORT_NAME "Astro"
    VERSION ${ASTRO_LIB_VERSION}
    SOVERSION ${ASTRO_LIB_SOVERSION}
)

target_include_directories(astro
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

# choose the correct target install library path
if(WIN32)
  INSTALL(TARGETS astro RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} ARCHIVE DESTINATION lib)
elseif(APPLE)
  INSTALL(TARGETS astro LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)
else()
  install(TARGETS astro
    EXPORT AstroTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
endif()

########### install headers & cmake config files ###############

if (WIN32 OR APPLE)
    # TODO: find how to install & deploy library, headers and config files as bundle for development
    return()
endif()


 INSTALL(FILES
    astrolib.h
    astr2lib.h
    attlib.h
    eclsolar.h
    planetarySats.h
    solarsystem.h
    ${CMAKE_CURRENT_BINARY_DIR}/astrolib_export.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/astro
)

########### CMake Config files ###############

include(CMakePackageConfigHelpers)

set(ASTRO_CMAKECONFIGNAME Astro)
set(ASTRO_INSTALL_CMAKECONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${ASTRO_CMAKECONFIGNAME})

configure_package_config_file(AstroConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ASTRO_CMAKECONFIGNAME}Config.cmake
    INSTALL_DESTINATION ${ASTRO_INSTALL_CMAKECONFIGDIR}
)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${ASTRO_CMAKECONFIGNAME}ConfigVersion.cmake
    VERSION ${ASTRO_LIB_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${ASTRO_CMAKECONFIGNAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${ASTRO_CMAKECONFIGNAME}ConfigVersion.cmake

    DESTINATION ${ASTRO_INSTALL_CMAKECONFIGDIR}
)

install(
    EXPORT AstroTargets
    FILE ${ASTRO_CMAKECONFIGNAME}Targets.cmake
    DESTINATION ${ASTRO_INSTALL_CMAKECONFIGDIR}
)
