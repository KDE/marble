PROJECT( StarsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib/astro
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(stars_SRCS StarsPlugin.cpp StarsPlugin.h)
set(stars_UI StarsConfigWidget.ui)

qt_wrap_ui(stars_SRCS  ${stars_UI})

marble_add_plugin( StarsPlugin ${stars_SRCS})
target_link_libraries(StarsPlugin astro)
