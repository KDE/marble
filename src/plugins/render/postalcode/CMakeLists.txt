PROJECT(PostalCode)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( postalcode_SRCS PostalCodePlugin.cpp
                     PostalCodePlugin.h
                     PostalCodeModel.cpp
                     PostalCodeModel.h
                     PostalCodeItem.cpp
                     PostalCodeItem.h )

set( PostalCode_LIBS Qt5::Script )

marble_add_plugin( PostalCode ${postalcode_SRCS} )
