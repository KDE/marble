project(MarbleThumbnailer)

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS KIO)

if (NOT KF6_FOUND)
  return()
endif()

set(marblethumbnail_SRCS
  thumbnailer.cpp
  thumbnailer.h
  main.cpp
)
add_library(marblethumbnail MODULE ${marblethumbnail_SRCS})
target_link_libraries(marblethumbnail
  marblewidget
  KF6::KIOGui
)
install(TARGETS marblethumbnail  DESTINATION ${KDE_INSTALL_PLUGINDIR})
