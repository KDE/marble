PROJECT(PostalCode)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( postalcode_SRCS PostalCodePlugin.cpp
                     PostalCodePlugin.h
                     PostalCodeModel.cpp
                     PostalCodeModel.h
                     PostalCodeItem.cpp
                     PostalCodeItem.h )

marble_add_plugin( PostalCode ${postalcode_SRCS} )
