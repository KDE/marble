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
)

target_link_libraries (TileCreator ${QT_QTMAIN_LIBRARY})

#      install (TARGETS marble DESTINATION ${BIN_INSTALL_DIR} )

