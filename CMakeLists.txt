project(marble)

option(QTONLY  "Build Qt only")

find_package(Qt4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

if (NOT QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (NOT QTONLY)

add_subdirectory(src)
add_subdirectory(data)

