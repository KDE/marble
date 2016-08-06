include_directories(
   ${CMAKE_SOURCE_DIR}/src/lib/marble/declarative
)

set(marblequick_SRCS
    qmlplugin.cpp
)

install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/marble/private/plasma)
add_library(marblequick SHARED ${marblequick_SRCS})

target_link_libraries(marblequick
    marbledeclarative
)

install(TARGETS marblequick DESTINATION ${QML_INSTALL_DIR}/org/kde/marble/private/plasma)
