project( RoutingInstructions )
include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
include( ${QT_USE_FILE} )

set( routing-instructions_SRC
        ../lib/routing/instructions/InstructionTransformation.cpp
        ../lib/routing/instructions/RoutingInstruction.cpp
        ../lib/routing/instructions/RoutingPoint.cpp
        ../lib/routing/instructions/RoutingWaypoint.cpp
        ../lib/routing/instructions/WaypointParser.cpp
        ../routing-instructions/main.cpp
)

qt4_automoc(${routing-instructions_SRC})

add_definitions( -DMAKE_MARBLE_LIB )
add_executable( routing-instructions ${routing-instructions_SRC} )
target_link_libraries( routing-instructions ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} )
if (APPLE)
  target_link_libraries (routing-instructions ${APP_SERVICES_LIBRARY})
endif (APPLE)

if(WIN32 AND QTONLY)
    install( TARGETS routing-instructions RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
else(WIN32 AND QTONLY)
    if (APPLE AND QTONLY)
      # No need for this when installing to a bundle
    else (APPLE AND QTONLY)
      if( NOT QTONLY)
        install( TARGETS routing-instructions ${INSTALL_TARGETS_DEFAULT_ARGS} )
      else( NOT QTONLY)
        install( TARGETS routing-instructions RUNTIME DESTINATION bin )
      endif(NOT QTONLY)
    endif (APPLE AND QTONLY)
endif(WIN32 AND QTONLY)
