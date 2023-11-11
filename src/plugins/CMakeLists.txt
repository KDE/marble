
# Logging for Marble plugins
ecm_qt_export_logging_category(
  IDENTIFIER "MARBLE_PLUGINS"
  CATEGORY_NAME "marble_plugins"
  EXPORT marble
  DESCRIPTION "Marble (plugins)")

add_definitions("-DLOGGING_IDENTIFIER=MARBLE_PLUGINS")

if(NOT ANDROID)
    add_subdirectory( designer )
    add_subdirectory( templates )
endif()

add_subdirectory( render )
add_subdirectory( positionprovider )
add_subdirectory( runner )
