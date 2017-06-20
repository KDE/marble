if (WITH_DESIGNER_PLUGIN)

  # TODO: find why this is not listed in the feature_summary (same with Qt5UiPlugin)
  find_package(Qt5 ${REQUIRED_QT_VERSION} QUIET COMPONENTS Designer)
  marble_set_package_properties( Qt5Designer PROPERTIES TYPE OPTIONAL PURPOSE "Required to build Qt Designer plugins" )

if(NOT Qt5Designer_VERSION VERSION_LESS 5.5.0)
    find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG QUIET COMPONENTS UiPlugin)
    set_package_properties(Qt5UiPlugin PROPERTIES TYPE OPTIONAL
        PURPOSE "Required to build Qt Designer plugins"
    )
    if (Qt5UiPlugin_FOUND)
        # for some reason, Qt5UiPlugin does not set its _INCLUDE_DIRS variable. Fill it manually
        get_target_property(Qt5UiPlugin_INCLUDE_DIRS Qt5::UiPlugin INTERFACE_INCLUDE_DIRECTORIES)
    endif()
    set(DesignerPluginDeps_FOUND ${Qt5UiPlugin_FOUND})
else()
    set(DesignerPluginDeps_FOUND ${Qt5Designer_FOUND})
endif()

  if (DesignerPluginDeps_FOUND)
    add_subdirectory( marblewidget )
    add_subdirectory( latloneditwidget )
    add_subdirectory( navigator )
  endif()
endif (WITH_DESIGNER_PLUGIN)
