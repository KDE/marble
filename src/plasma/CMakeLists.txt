set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS Config Plasma Service)

if (NOT KF5_FOUND)
  return()
endif()


add_subdirectory(qmlplugin)

add_subdirectory(applets)
add_subdirectory(wallpapers)
