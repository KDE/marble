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

macro( marble_qt4_automoc )
  if( ${CMAKE_VERSION} STRLESS "2.8" OR QT4_FOUND)
    qt4_automoc( ${ARGN} )
  else()
    # Just ignore it
  endif()
endmacro()

macro(qt_add_resources)
  if( QT4_FOUND )
    qt4_add_resources(${ARGN})
  else()
    qt5_add_resources(${ARGN})
  endif()
endmacro()

macro(qt_wrap_ui)
  if( QT4_FOUND )
    qt4_wrap_ui(${ARGN})
  else()
    qt5_wrap_ui(${ARGN})
  endif()
endmacro()

macro(qt_generate_moc)
  if( QT4_FOUND )
    qt4_generate_moc(${ARGN})
  else()
    qt5_generate_moc(${ARGN})
  endif()
endmacro()

# the place to put in common cmake macros
# this is needed to minimize the amount of errors to do
macro( marble_add_plugin _target_name )
set( _src ${ARGN} )
if( QTONLY )
    marble_qt4_automoc( ${_src} )
    add_library( ${_target_name} MODULE ${_src} )
    target_link_libraries( ${_target_name} ${QT_QTCORE_LIBRARY}
                                           ${QT_QTDBUS_LIBRARY}
                                           ${QT_QTGUI_LIBRARY}
                                           ${QT_QTXML_LIBRARY}
                                           ${QT_QTSVG_LIBRARY}
                                           ${QT_QTNETWORK_LIBRARY}
                                           ${QT_QTMAIN_LIBRARY}
                                           ${${_target_name}_LIBS}
                                           marblewidget )
    install( TARGETS ${_target_name} DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH} )
else( QTONLY )
    kde4_add_plugin( ${_target_name} ${_src} )
    target_link_libraries( ${_target_name} ${QT_QTCORE_LIBRARY}
                                           ${QT_QTDBUS_LIBRARY}
                                           ${QT_QTGUI_LIBRARY}
                                           ${QT_QTXML_LIBRARY}
                                           ${QT_QTSVG_LIBRARY}
                                           ${QT_QTNETWORK_LIBRARY}
                                           ${KDE4_KDECORE_LIBRARY}
                                           ${KDE4_KDEUI_LIBRARY}
                                           ${KDE4_KIO_LIBRARY}
                                           ${QT_QTMAIN_LIBRARY}
                                           ${${_target_name}_LIBS}
                                           marblewidget )
    install( TARGETS ${_target_name} DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH} )
endif( QTONLY )

set_target_properties( ${_target_name} PROPERTIES 
                       INSTALL_RPATH_USE_LINK_PATH TRUE  
                       SKIP_BUILD_RPATH TRUE 
                       BUILD_WITH_INSTALL_RPATH TRUE 
                     )

endmacro( marble_add_plugin _target_name )

# these plugins are slightly different

macro( marble_add_designer_plugin _target_name )
set( _src ${ARGN} )

qt_add_resources( _src ../../../apps/marble-ui/marble.qrc )

if( QTONLY )
    marble_qt4_automoc( ${_src} )
    add_library( ${_target_name} MODULE ${_src} )
    target_link_libraries( ${_target_name} ${QT_QTCORE_LIBRARY}
                                           ${QT_QTDBUS_LIBRARY}
                                           ${QT_QTGUI_LIBRARY}
                                           ${QT_QTXML_LIBRARY}
                                           ${QT_QTSVG_LIBRARY}
                                           ${QT_QTNETWORK_LIBRARY}
                                           ${QT_QTMAIN_LIBRARY}
                                           ${${_target_name}_LIBS}
                                           marblewidget )
    install( TARGETS ${_target_name} DESTINATION ${QT_PLUGINS_DIR}/designer )
else( QTONLY )
    kde4_add_plugin( ${_target_name} ${_src} )
    target_link_libraries( ${_target_name} ${QT_QTCORE_LIBRARY}
                                           ${QT_QTDBUS_LIBRARY}
                                           ${QT_QTGUI_LIBRARY}
                                           ${QT_QTXML_LIBRARY}
                                           ${QT_QTSVG_LIBRARY}
                                           ${QT_QTNETWORK_LIBRARY}
                                           ${KDE4_KDECORE_LIBRARY}
                                           ${KDE4_KDEUI_LIBRARY}
                                           ${KDE4_KIO_LIBRARY}
                                           ${QT_QTMAIN_LIBRARY}
                                           ${${_target_name}_LIBS}
                                           marblewidget )
    install( TARGETS ${_target_name} DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )
endif( QTONLY )

set_target_properties( ${_target_name} PROPERTIES 
                       INSTALL_RPATH_USE_LINK_PATH TRUE  
                       SKIP_BUILD_RPATH TRUE 
                       BUILD_WITH_INSTALL_RPATH TRUE 
                     )

endmacro( marble_add_designer_plugin _target_name )

macro( marble_add_declarative_plugin _target_name _install_path )
set( _src ${ARGN} )
marble_qt4_automoc( ${_src} )
add_library( ${_target_name} MODULE ${_src} )
target_link_libraries( ${_target_name} ${QT_QTCORE_LIBRARY}
                                           ${QT_QTDBUS_LIBRARY}
                                           ${QT_QTGUI_LIBRARY}
                                           ${QT_QTXML_LIBRARY}
                                           ${QT_QTSVG_LIBRARY}
                                           ${QT_QTNETWORK_LIBRARY}
                                           ${QT_QTMAIN_LIBRARY}
                                           ${${_target_name}_LIBS}
                                           marblewidget )
install( TARGETS ${_target_name} DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/${_install_path} )

set_target_properties( ${_target_name} PROPERTIES
                       INSTALL_RPATH_USE_LINK_PATH TRUE
                       SKIP_BUILD_RPATH TRUE
                       BUILD_WITH_INSTALL_RPATH TRUE
                     )

endmacro( marble_add_declarative_plugin _target_name )

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
        if( QTONLY )
            qt_generate_moc( ${TEST_NAME}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc )
            include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
            set( ${TEST_NAME}_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc ${${TEST_NAME}_SRCS} )
          
            add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
        else( QTONLY )
            kde4_add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
        endif( QTONLY )
        target_link_libraries( ${TEST_NAME} ${QT_QTMAIN_LIBRARY}
                                            ${QT_QTCORE_LIBRARY} 
                                            ${QT_QTGUI_LIBRARY} 
                                            ${QT_QTTEST_LIBRARY} 
                                            ${Qt5Test_LIBRARIES}
                                            marblewidget )
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
