PROJECT(Weather)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/weather
 ${CMAKE_BINARY_DIR}/src/plugins/render/weather
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

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

qt4_wrap_ui( weather_SRCS ${weather_UI} )

marble_add_plugin( Weather ${weather_SRCS} )
target_link_libraries( Weather ${QT_QTWEBKIT_LIBRARY} )
