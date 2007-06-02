
PROJECT( MarbleNavigatorPlugin )
cmake_minimum_required(VERSION 2.4.0)

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/plugins/navigator
 ${CMAKE_BINARY_DIR}/src/plugins/navigator
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set(marblenavigator_SRCS
           MarbleNavigator.cpp
           MarbleNavigatorPlugin.cpp
)
MESSAGE( STATUS ${marblenavigator_SRCS} )

set(marblenavigator_HDRS
           MarbleNavigator.h
           MarbleNavigatorPlugin.h
)
MESSAGE( STATUS ${marblenavigator_HDRS} )

set(marblenavigator_UI
           MarbleNavigator.ui
)
MESSAGE( STATUS ${marblenavigator_UI} )

qt4_add_resources(marblenavigator_SRCS ../../marble.qrc)

if (QTONLY)
  qt4_wrap_ui(marblenavigator_SRCS ${marblenavigator_UI})
  qt4_automoc(${marblenavigator_SRCS})
else (QTONLY)
  kde4_add_ui_files(marblenavigator_SRCS ${marblenavigator_UI})
  kde4_automoc(${marblenavigator_SRCS})
endif (QTONLY)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_DLL)

ADD_LIBRARY(MarbleNavigatorPlugin SHARED ${marblenavigator_SRCS})

if (QTONLY)
    TARGET_LINK_LIBRARIES (MarbleNavigatorPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY})
else (QTONLY)
    TARGET_LINK_LIBRARIES (MarbleNavigatorPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY})
endif (QTONLY)
if(WIN32)
  TARGET_LINK_LIBRARIES(MarbleNavigatorPlugin ws2_32 imm32 winmm)
endif(WIN32)

set_target_properties(MarbleNavigatorPlugin PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE  SKIP_BUILD_RPATH TRUE BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS MarbleNavigatorPlugin DESTINATION ${QT_PLUGINS_DIR}/designer)

