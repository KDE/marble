if (WITH_DESIGNER_PLUGIN)

  # TODO: find why this is not listed in the feature_summary (same with Qt6UiPlugin)
  find_package(Qt6 ${REQUIRED_QT_VERSION} QUIET COMPONENTS Designer)
  marble_set_package_properties( Qt6Designer PROPERTIES TYPE OPTIONAL PURPOSE "Required to build Qt Designer plugins" )

if(NOT Qt6Designer_VERSION VERSION_LESS 5.5.0)
    find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG QUIET COMPONENTS UiPlugin)
    set_package_properties(Qt6UiPlugin PROPERTIES TYPE OPTIONAL
        PURPOSE "Required to build Qt Designer plugins"
    )
    if (Qt6UiPlugin_FOUND)
        # for some reason, Qt6UiPlugin does not set its _INCLUDE_DIRS variable. Fill it manually
        get_target_property(Qt6UiPlugin_INCLUDE_DIRS Qt6::UiPlugin INTERFACE_INCLUDE_DIRECTORIES)
    endif()
    set(DesignerPluginDeps_FOUND ${Qt6UiPlugin_FOUND})
else()
    set(DesignerPluginDeps_FOUND ${Qt6Designer_FOUND})
endif()

  if (DesignerPluginDeps_FOUND)
    add_subdirectory( marblewidget )
    add_subdirectory( latloneditwidget )
    add_subdirectory( navigator )
  endif()
endif (WITH_DESIGNER_PLUGIN)
