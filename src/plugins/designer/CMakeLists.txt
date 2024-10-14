if(WITH_DESIGNER_PLUGIN)

  # TODO: find why this is not listed in the feature_summary (same with Qt6UiPlugin)
  find_package(Qt6 ${REQUIRED_QT_VERSION} QUIET COMPONENTS Designer)
  marble_set_package_properties( Qt6Designer PROPERTIES TYPE OPTIONAL PURPOSE "Required to build Qt Designer plugins")

  set(DesignerPluginDeps_FOUND ${Qt6Designer_FOUND})

  if(DesignerPluginDeps_FOUND)
    add_subdirectory(marblewidget)
    add_subdirectory(latloneditwidget)
    add_subdirectory(navigator)
  endif()
endif(WITH_DESIGNER_PLUGIN)
