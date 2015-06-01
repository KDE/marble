project(MarbleThumbnailer)

set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS KIO)

if (NOT KF5_FOUND)       
  return()
endif()

set(marblethumbnail_SRCS
  thumbnailer.cpp
  main.cpp
)
add_library(marblethumbnail MODULE ${marblethumbnail_SRCS})
target_link_libraries(marblethumbnail
  ${MARBLEWIDGET}
  KF5::KIOWidgets
)
install(TARGETS marblethumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})
