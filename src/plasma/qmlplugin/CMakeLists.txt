include_directories(
   ${CMAKE_SOURCE_DIR}/src/lib/marble/declarative
)

set(marblequick_SRCS
    qmlplugin.cpp
)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/marble/private/plasma)
add_library(marblequick SHARED ${marblequick_SRCS})

target_link_libraries(marblequick
    marbledeclarative
)

install(TARGETS marblequick DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/marble/private/plasma)
