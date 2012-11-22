PROJECT( LocalDatabasePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( localdatabase_SRCS LocalDatabaseRunner.cpp LocalDatabasePlugin.cpp )

marble_add_plugin( LocalDatabasePlugin ${localdatabase_SRCS} )
