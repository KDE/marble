include(FeatureSummary)
if( COMMAND set_package_properties )
  macro( marble_set_package_properties )
  set_package_properties( ${ARGN} )
  endmacro()
else()
  macro( marble_set_package_properties )
  # Just ignore it
  endmacro()
endif()

if ( COMMAND feature_summary )
  macro( marble_feature_summary )
  feature_summary( ${ARGN} )
  endmacro()
else()
  macro( marble_feature_summary )
  # Just ignore it
  endmacro()
endif()

macro(qt_add_resources)
  qt5_add_resources(${ARGN})
endmacro()

macro(qt_wrap_ui)
  qt5_wrap_ui(${ARGN})
endmacro()

macro(qt_generate_moc)
  qt5_generate_moc(${ARGN})
endmacro()

# the place to put in common cmake macros
# this is needed to minimize the amount of errors to do
macro( marble_add_plugin _target_name )
set( _src ${ARGN} )
add_library( ${_target_name} MODULE ${_src} )
target_link_libraries( ${_target_name} ${${_target_name}_LIBS}
                                       marblewidget )
install( TARGETS ${_target_name} DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH} )

endmacro( marble_add_plugin _target_name )

# these plugins are slightly different

macro( marble_add_designer_plugin _target_name )
set( _src ${ARGN} )

qt_add_resources( _src ../../../apps/marble-ui/marble.qrc )

add_library( ${_target_name} MODULE ${_src} )
target_link_libraries( ${_target_name} ${${_target_name}_LIBS}
                                       marblewidget )
install( TARGETS ${_target_name} DESTINATION ${QT_PLUGINS_DIR}/designer )

endmacro( marble_add_designer_plugin _target_name )

if( WIN32 )
    set( DATA_PATH ${CMAKE_INSTALL_PREFIX}/${MARBLE_DATA_PATH} )
    set( PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${MARBLE_PLUGIN_PATH} )
else( WIN32 )
    set( DATA_PATH ${MARBLE_DATA_PATH} )
    set( PLUGIN_PATH ${MARBLE_PLUGIN_PATH} )
endif( WIN32 )

macro( marble_add_test TEST_NAME )
    if( BUILD_MARBLE_TESTS )
        set( ${TEST_NAME}_SRCS ${TEST_NAME}.cpp ${ARGN} )
        qt_generate_moc( ${TEST_NAME}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc )
        include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
        set( ${TEST_NAME}_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc ${${TEST_NAME}_SRCS} )

        add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
        target_link_libraries(${TEST_NAME}
            marblewidget
            Qt5::Test
        )

        set_target_properties( ${TEST_NAME} PROPERTIES 
                               COMPILE_FLAGS "-DDATA_PATH=\"\\\"${DATA_PATH}\\\"\" -DPLUGIN_PATH=\"\\\"${PLUGIN_PATH}\\\"\"" )
        add_test( ${TEST_NAME} ${TEST_NAME} )
    endif( BUILD_MARBLE_TESTS )
endmacro( marble_add_test TEST_NAME )

macro( marble_add_project_resources resources )
  add_custom_target( ${PROJECT_NAME}_Resources ALL SOURCES ${ARGN} )
endmacro()

# - MACRO_OPTIONAL_FIND_PACKAGE() combines FIND_PACKAGE() with an OPTION()
# MACRO_OPTIONAL_FIND_PACKAGE( <name> [QUIT] )
# This macro is a combination of OPTION() and FIND_PACKAGE(), it
# works like FIND_PACKAGE(), but additionally it automatically creates
# an option name WITH_<name>, which can be disabled via the cmake GUI.
# or via -DWITH_<name>=OFF
# The standard <name>_FOUND variables can be used in the same way
# as when using the normal FIND_PACKAGE()

# Copyright (c) 2006-2010 Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This is just a helper macro to set a bunch of variables empty.
# We don't know whether the package uses UPPERCASENAME or CamelCaseName, so we try both:

if(NOT COMMAND _MOFP_SET_EMPTY_IF_DEFINED)
macro(_MOFP_SET_EMPTY_IF_DEFINED _name _var)
   if(DEFINED ${_name}_${_var})
      set(${_name}_${_var} "")
   endif(DEFINED ${_name}_${_var})

   string(TOUPPER ${_name} _nameUpper)
   if(DEFINED ${_nameUpper}_${_var})
      set(${_nameUpper}_${_var}  "")
   endif(DEFINED ${_nameUpper}_${_var})
endmacro(_MOFP_SET_EMPTY_IF_DEFINED _package _var)
endif()

if(NOT COMMAND MACRO_OPTIONAL_FIND_PACKAGE)
macro (MACRO_OPTIONAL_FIND_PACKAGE _name )
   option(WITH_${_name} "Search for ${_name} package" ON)
   if (WITH_${_name})
      find_package(${_name} ${ARGN})
   else (WITH_${_name})
      string(TOUPPER ${_name} _nameUpper)
      set(${_name}_FOUND FALSE)
      set(${_nameUpper}_FOUND FALSE)

      _mofp_set_empty_if_defined(${_name} INCLUDE_DIRS)
      _mofp_set_empty_if_defined(${_name} INCLUDE_DIR)
      _mofp_set_empty_if_defined(${_name} INCLUDES)
      _mofp_set_empty_if_defined(${_name} LIBRARY)
      _mofp_set_empty_if_defined(${_name} LIBRARIES)
      _mofp_set_empty_if_defined(${_name} LIBS)
      _mofp_set_empty_if_defined(${_name} FLAGS)
      _mofp_set_empty_if_defined(${_name} DEFINITIONS)
   endif (WITH_${_name})
endmacro (MACRO_OPTIONAL_FIND_PACKAGE)
endif()

# older cmake version don't have the add_feature_info macro.
# It's just informative, so we add our own that does
# nothing in that case
if(NOT COMMAND ADD_FEATURE_INFO)
macro(ADD_FEATURE_INFO)
  # just ignore it
endmacro()
endif()


# Find Qt translation tools
find_package(Qt5LinguistTools CONFIG)

if(NOT Qt5LinguistTools_FOUND)

# dummy implementation
function(marble_install_po_files_as_qm podir)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARGS_TARGET)
        if(NOT TARGET ${ARGS_TARGET})
            add_custom_target(${ARGS_TARGET})
        endif()
    endif()
    if (NOT IS_ABSOLUTE "${podir}")
        set(podir "${CMAKE_CURRENT_SOURCE_DIR}/${podir}")
    endif()
    if (IS_DIRECTORY "${podir}")
        message(STATUS "Will do nothing for po files in \"${podir}\", tools missing")
    endif()
endfunction()

else()

if(TARGET Qt5::lconvert)
    set(lconvert_executable Qt5::lconvert)
else()
    # Qt < 5.3.1 does not define Qt5::lconvert
    get_target_property(lrelease_location Qt5::lrelease LOCATION)
    get_filename_component(lrelease_path ${lrelease_location} PATH)
    find_program(lconvert_executable
        NAMES lconvert-qt5 lconvert
        PATHS ${lrelease_path}
        NO_DEFAULT_PATH
    )
endif()

function(marble_process_po_files_as_qm lang po_file)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create commands to turn po files into qm files
    get_filename_component(po_file ${po_file} ABSOLUTE)
    get_filename_component(filename_base ${po_file} NAME_WE)

    # Include ${lang} in build dir because we might be called multiple times
    # with the same ${filename_base}
    set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/locale/${lang})
    set(ts_file ${build_dir}/${filename_base}.ts)
    set(qm_file ${build_dir}/${filename_base}.qm)
    if(CMAKE_SYSTEM_NAME STREQUAL Android)
        set(qm_install_dir "${locale_dir}/${lang}")
    else()
        set(qm_install_dir "${locale_dir}/${lang}/LC_MESSAGES")
    endif()

    file(MAKE_DIRECTORY ${build_dir})

    # lconvert from .po to .ts, then lrelease from .ts to .qm.
    add_custom_command(OUTPUT ${qm_file}
        COMMAND ${lconvert_executable}
            ARGS -i ${po_file} -o ${ts_file} -target-language ${lang}
        COMMAND Qt5::lrelease
            ARGS -removeidentical -nounfinished -silent ${ts_file} -qm ${qm_file}
        DEPENDS ${po_file}
    )
    install(
        FILES ${qm_file}
        DESTINATION ${qm_install_dir}
        OPTIONAL # if not build, ignore it
    )

    if(ARGS_TARGET)
        set(target_name_prefix ${ARGS_TARGET})
    else()
        set(target_name_prefix translation)
    endif()
    string(REPLACE "@" "AT" _lang ${lang})
    set(target_name ${target_name_prefix}_${_lang}_${filename_base})

    if(ARGS_TARGET)
        add_custom_target(${target_name} DEPENDS ${qm_file})
        add_dependencies(${ARGS_TARGET} ${target_name})
    else()
        add_custom_target(${target_name} ALL DEPENDS ${qm_file})
    endif()
endfunction()


function(marble_install_po_files_as_qm podir)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARGS_TARGET)
        if(NOT TARGET ${ARGS_TARGET})
            add_custom_target(${ARGS_TARGET})
        endif()

        set(target_arg TARGET ${ARGS_TARGET})
    else()
        set(target_arg)
    endif()

    file(GLOB po_files "${podir}/*/*.po")
    foreach(po_file ${po_files})
        get_filename_component(po_dir ${po_file} DIRECTORY)
        get_filename_component(lang ${po_dir} NAME)
        marble_process_po_files_as_qm(${lang} ${po_file} ${target_arg})
    endforeach()
endfunction()

endif()
