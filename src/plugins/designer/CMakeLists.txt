if (WITH_DESIGNER_PLUGIN)

  if(QT5BUILD)
    find_package(Qt5Designer REQUIRED)
  endif()

  add_subdirectory( marblewidget )
  add_subdirectory( latloneditwidget )
  add_subdirectory( navigator )
endif (WITH_DESIGNER_PLUGIN)

