macro_optional_find_package(Plasma "6.0.0" QUIET)

if (NOT Plasma_FOUND)
  return()
endif()

add_subdirectory(applets)
add_subdirectory(wallpapers)
