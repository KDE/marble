PROJECT(Weather)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

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

set(Weather_LIBS Qt5::Svg)

marble_add_plugin( Weather ${weather_SRCS} )
