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
                  BBCParser.cpp
                  BBCWeatherService.cpp
                  BBCWeatherItem.cpp
                  FakeWeatherService.cpp 
                  FakeWeatherItem.cpp
                  StationListParser.cpp )

marble_add_plugin( Weather ${weather_SRCS} )
target_link_libraries( Weather ${QT_QTGUI_LIBRARY} )
