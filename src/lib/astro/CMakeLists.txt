PROJECT(astro VERSION ${RELEASE_SERVICE_VERSION})

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

SET(astrolib_SRCS
  astrolib.cpp
  astr2lib.cpp
  attlib.cpp
  eclsolar.cpp
  planetarySats.cpp
  solarsystem.cpp

  astrolib.h
  astr2lib.h
  attlib.h
  eclsolar.h
  planetarySats.h
  solarsystem.h
)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX ASTRO
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/astro_version.h
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/AstroConfigVersion.cmake"
    SOVERSION 1
)

ADD_LIBRARY(astro ${astrolib_SRCS})
generate_export_header(astro BASE_NAME astrolib)
target_include_directories(astro PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

if(APPLE)
  #defined in top level makefile
  TARGET_LINK_LIBRARIES(astro ${MAC_EXTRA_LIBS})
endif(APPLE)

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  TARGET_LINK_LIBRARIES(astro m)
endif(CMAKE_SYSTEM_NAME MATCHES "SunOS")

if(WIN32)
  TARGET_LINK_LIBRARIES(astro ws2_32 imm32 winmm)
endif(WIN32)

if(NOT ANDROID)
  set_target_properties(astro PROPERTIES
      EXPORT_NAME "Astro"
      VERSION ${ASTRO_VERSION}
      SOVERSION ${ASTRO_SOVERSION}
)
endif()

target_include_directories(astro
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

# choose the correct target install library path
if(WIN32)
  INSTALL(TARGETS astro EXPORT AstroTargets RUNTIME DESTINATION . ARCHIVE DESTINATION lib)
elseif(APPLE)
  INSTALL(TARGETS astro LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)
else()
  install(TARGETS astro
    EXPORT AstroTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
endif()

########### install headers & cmake config files ###############

if(APPLE)
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
    ${CMAKE_CURRENT_BINARY_DIR}/astro_version.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/astro
)

########### CMake Config files ###############

include(CMakePackageConfigHelpers)

set(ASTRO_INSTALL_CMAKECONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/Astro")

configure_package_config_file(AstroConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AstroConfig.cmake
    INSTALL_DESTINATION ${ASTRO_INSTALL_CMAKECONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AstroConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/AstroConfigVersion.cmake
    DESTINATION ${ASTRO_INSTALL_CMAKECONFIGDIR}
    COMPONENT Devel
)

install(
    EXPORT AstroTargets
    FILE AstroTargets.cmake
    DESTINATION ${ASTRO_INSTALL_CMAKECONFIGDIR}
)
