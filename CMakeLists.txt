project(marble)

option(QTONLY  "Build Qt only" OFF)
option(STATIC_BUILD "Link to static Qt libs (win32 only)" OFF)
if(STATIC_BUILD)
    add_definitions(-DSTATIC_BUILD=1)
endif(STATIC_BUILD)

# Variables to test new kml implementation
# Will removed in feature after success
#add_definitions(-DKML_DEBUG)
#add_definitions(-DKML_GSOC)

if(QTONLY)
  find_package(Qt4 REQUIRED)
endif(QTONLY)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_CURRENT_SOURCE_DIR})

include(DistTarget)

if (QTONLY)
  # add a flag to be able to distinguish between qt and kde mode in the sources
  add_definitions(-DQTONLY)
else(QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (QTONLY)

add_subdirectory(src)
add_subdirectory(data)

if(WIN32)
  set(data_dir data)
else(WIN32)
  if (QTONLY)
    set(data_dir ${CMAKE_INSTALL_PREFIX}/share/apps/marble/data)
  else (QTONLY)
    set(data_dir ${DATA_INSTALL_DIR}/marble/data)
  endif (QTONLY)
endif(WIN32)

install(FILES LICENSE.txt DESTINATION ${data_dir})
