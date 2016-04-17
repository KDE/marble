PROJECT( License )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( license_SRCS License.cpp )

marble_add_plugin( License ${license_SRCS} )
