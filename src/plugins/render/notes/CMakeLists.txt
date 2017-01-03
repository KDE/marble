PROJECT(Notes)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( notes_SRCS NotesPlugin.cpp
                     NotesPlugin.h
                     NotesModel.cpp
                     NotesModel.h
                     NotesItem.cpp
                     NotesItem.h)

marble_add_plugin( NotesPlugin ${notes_SRCS} )

