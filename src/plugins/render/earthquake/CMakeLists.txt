PROJECT( EarthquakePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4BUILD )
  INCLUDE(${QT_USE_FILE})
else()
  INCLUDE_DIRECTORIES(
   ${Qt5Script_INCLUDE_DIRS})
endif()

set( my_SRCS
  EarthquakePlugin.cpp
  EarthquakeModel.cpp
  EarthquakeItem.cpp
 )
set( earthquake_UI EarthquakeConfigWidget.ui )

qt_wrap_ui(my_SRCS ${earthquake_UI})
qt_add_resources(my_SRCS earthquake.qrc)

marble_add_plugin( EarthquakePlugin ${my_SRCS} )
