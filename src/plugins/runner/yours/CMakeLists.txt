PROJECT( YoursPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

INCLUDE_DIRECTORIES(
  ${Qt5WebKitWidgets_INCLUDE_DIRS}
  ${Qt5Xml_INCLUDE_DIRS}
)

set( yours_SRCS YoursRunner.cpp YoursPlugin.cpp )

marble_add_plugin( YoursPlugin ${yours_SRCS} )
