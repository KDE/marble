if(EXPERIMENTAL_PYTHON_BINDINGS)
  MESSAGE(WARNING "Marble's Python bindings lack a maintainer and therefore had to be disabled. Do you need them? Please contact us at https://marble.kde.org/support.php")
  return()
endif()

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS Wallet NewStuff Parts)

if(NOT KF5_FOUND)
  return()
endif()

#### Python support ####
set(PythonSupport_FOUND FALSE)

set_package_properties(EXPERIMENTAL_PYTHON_BINDINGS PROPERTIES
                       URL "http://techbase.kde.org/Development/Languages/Python"
                       TYPE OPTIONAL
                       PURPOSE "Experimental Python binding support for the Marble library. To activate it pass -DEXPERIMENTAL_PYTHON_BINDINGS=TRUE to cmake."
                      )

if(EXPERIMENTAL_PYTHON_BINDINGS)
    macro_optional_find_package(PythonLibrary)

    macro_optional_find_package(SIP)
    if(SIP_FOUND AND SIP_VERSION STRLESS "040c02")
        message(STATUS "The version of SIP found is too old. 4.12.2 or later is needed.")
        set(SIP_FOUND)
    endif(SIP_FOUND AND SIP_VERSION STRLESS "040c02")

    include(SIPMacros)

    macro_optional_find_package(PyQt4)
    if(PYQT4BUILD AND PYQT4_VERSION STRLESS "040804")
        message(STATUS "The version of PyQt found is too old. 4.8.4 or later is required.")
        set(PYQT4BUILD)
    endif(PYQT4BUILD AND PYQT4_VERSION STRLESS "040804")
    set_package_properties(PYTHONQT4BUILD PROPERTIES
                             URL "http://www.riverbankcomputing.co.uk/software/pyqt/intro"
                             TYPE OPTIONAL
                             PURPOSE "PyQt4"
                            )

    if (PYTHONLIBRARY_FOUND AND SIP_FOUND AND PYQT4BUILD)
      set(PythonSupport_FOUND TRUE)
    endif (PYTHONLIBRARY_FOUND AND SIP_FOUND AND PYQT4BUILD)

    set_package_properties(PythonSupport_FOUND PROPERTIES
                              URL "http://techbase.kde.org/Development/Languages/Python"
                              TYPE OPTIONAL
                              PURPOSE "Needed for Python bindings to the marble widget."
                             )
endif(EXPERIMENTAL_PYTHON_BINDINGS)

#### End Python support ####


if(PythonSupport_FOUND)
  macro_optional_add_subdirectory(python)
endif(PythonSupport_FOUND)
