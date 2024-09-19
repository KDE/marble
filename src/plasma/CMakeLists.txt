macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS Plasma)

if (NOT KF6_FOUND)
  return()
endif()


add_subdirectory(qmlplugin)

add_subdirectory(applets)
add_subdirectory(wallpapers)
