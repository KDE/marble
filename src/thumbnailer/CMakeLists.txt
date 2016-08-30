project(MarbleThumbnailer)

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS KIO)

if (NOT KF5_FOUND)
  return()
endif()

set(marblethumbnail_SRCS
  thumbnailer.cpp
  main.cpp
)
add_library(marblethumbnail MODULE ${marblethumbnail_SRCS})
target_link_libraries(marblethumbnail
  marblewidget
  KF5::KIOWidgets
)
install(TARGETS marblethumbnail  DESTINATION ${KDE_INSTALL_PLUGINDIR})
