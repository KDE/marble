if (WITH_DESIGNER_PLUGIN)

  find_package(Qt5Designer)

  add_subdirectory( marblewidget )
  add_subdirectory( latloneditwidget )
  add_subdirectory( navigator )
endif (WITH_DESIGNER_PLUGIN)

