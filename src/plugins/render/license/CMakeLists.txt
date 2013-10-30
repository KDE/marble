PROJECT( License )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
INCLUDE(${QT_USE_FILE})
endif()

set( license_SRCS License.cpp )

marble_qt4_automoc( ${license_SRCS} )

marble_add_plugin( License ${license_SRCS} )
