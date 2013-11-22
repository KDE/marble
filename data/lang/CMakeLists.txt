SET (TARGET merge_ts_po)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  include( ${QT_USE_FILE} )
endif()

set( ${TARGET}_SRC merge_ts_po.cpp )
add_executable( ${TARGET} ${${TARGET}_SRC} )

if (QT4_FOUND)
  target_link_libraries( ${TARGET} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} )
else()
  target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} )
endif()

# MARBLE_WRAP_PO(qmfiles pofile1 pofile2 ... )
MACRO(MARBLE_WRAP_PO qmfiles)
  FILE(GLOB_RECURSE all_sources RELATIVE "${CMAKE_SOURCE_DIR}/src/" "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.ui")
  SET(tstemplate ${CMAKE_CURRENT_BINARY_DIR}/template.ts)
  ADD_CUSTOM_COMMAND(OUTPUT ${tstemplate}
    COMMAND ${QT_LUPDATE_EXECUTABLE}
    ARGS ${all_sources} -ts ${tstemplate}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
  )
  FOREACH (pofile ${ARGN})
    GET_FILENAME_COMPONENT(pofile ${pofile} ABSOLUTE)
    GET_FILENAME_COMPONENT(basename ${pofile} NAME_WE)
    SET(tsfile "${CMAKE_CURRENT_BINARY_DIR}/${basename}.ts")
    SET(qmfile "${CMAKE_CURRENT_BINARY_DIR}/${basename}.qm")
    ADD_CUSTOM_COMMAND(OUTPUT ${tsfile}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/merge_ts_po
      ARGS ${tstemplate} ${pofile} > ${tsfile}
      OUTPUT ${qmfile}
      COMMAND ${QT_LRELEASE_EXECUTABLE}
      ARGS -silent ${tsfile} -qm ${qmfile}
      DEPENDS ${TARGET} ${tstemplate} ${pofile}
    )
    SET(${qmfiles} ${${qmfiles}} "${qmfile}")
  ENDFOREACH(pofile)
ENDMACRO(MARBLE_WRAP_PO)


FILE(GLOB LANG_SRC "*.po")

# Create our translation files.
MARBLE_WRAP_PO(LANG_FILES ${LANG_SRC})

ADD_CUSTOM_TARGET(translations ALL
  DEPENDS ${LANG_FILES}
)

install(FILES ${LANG_FILES} DESTINATION ${MARBLE_DATA_INSTALL_PATH}/lang)
