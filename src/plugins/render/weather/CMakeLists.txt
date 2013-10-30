PROJECT(Weather)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
else()
 INCLUDE_DIRECTORIES(${Qt5WebKitWidgets_INCLUDE_DIRS})
endif()

set( weather_SRCS WeatherData.cpp
                  WeatherItem.cpp
                  WeatherModel.cpp 
                  WeatherPlugin.cpp
                  AbstractWeatherService.cpp
                  BBCItemGetter.cpp
                  BBCParser.cpp
                  BBCStation.cpp
                  BBCWeatherService.cpp
                  BBCWeatherItem.cpp
                  FakeWeatherService.cpp 
                  FakeWeatherItem.cpp
                  StationListParser.cpp
                  GeoNamesWeatherService.cpp
                  GeoNamesWeatherItem.cpp )
set( weather_UI WeatherConfigWidget.ui )

qt_wrap_ui( weather_SRCS ${weather_UI} )
qt_add_resources( weather_SRCS weather.qrc )

marble_add_plugin( Weather ${weather_SRCS} )
target_link_libraries( Weather ${QT_QTWEBKIT_LIBRARY} )
