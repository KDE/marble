PROJECT(TileCreator)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/tilecreator
 ${CMAKE_BINARY_DIR}/src/tilecreator
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set(tilecreator_SRCS
            main.cpp
            tccore.cpp
)

add_executable(tilecreator ${tilecreator_SRCS})
target_link_libraries(tilecreator ${QT_QTCORE_LIBRARY} marblewidget)

install (TARGETS tilecreator RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

