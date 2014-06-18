project(MarbleThumbnailer)

include_directories(
  ${KDE4_INCLUDES}
)

set(marblethumbnail_SRCS
  thumbnailer.cpp
  main.cpp
)
kde4_add_plugin(marblethumbnail ${marblethumbnail_SRCS})
target_link_libraries(marblethumbnail
  marblewidget
  ${KDE4_KIO_LIBS}
)
install(TARGETS marblethumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})
