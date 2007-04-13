project(marble)

option(QTONLY  "Build Qt only" ON)
option(STATIC_BUILD "Link to static Qt libs (win32 only)" OFF)
if(STATIC_BUILD)
    add_definitions(-DSTATIC_BUILD=1)
endif(STATIC_BUILD)

find_package(Qt4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_SOURCE_DIR})

include(DistTarget)

if (NOT QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (NOT QTONLY)

add_subdirectory(src)
add_subdirectory(data)
