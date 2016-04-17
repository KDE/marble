#
# Template for Float Items.
#
# Change all occurrences of 'FITemplate' below to the name of your new
# template.  (Also remove this comment.)
#

PROJECT(FITemplateFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( FITemplate_SRCS FITemplateFloatItem.cpp )

marble_add_plugin( FITemplateFloatItem ${FITemplate_SRCS} )
