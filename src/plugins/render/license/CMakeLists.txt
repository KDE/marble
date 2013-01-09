PROJECT( License )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( license_SRCS License.cpp )

qt4_automoc( ${license_SRCS} )

marble_add_plugin( License ${license_SRCS} )
