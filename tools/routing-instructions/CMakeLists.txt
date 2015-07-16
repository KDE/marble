project( RoutingInstructions )
include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4BUILD )
  include( ${QT_USE_FILE} )
endif()

set( routing-instructions_SRC
        ../../src/lib/marble/routing/instructions/InstructionTransformation.cpp
        ../../src/lib/marble/routing/instructions/RoutingInstruction.cpp
        ../../src/lib/marble/routing/instructions/RoutingPoint.cpp
        ../../src/lib/marble/routing/instructions/RoutingWaypoint.cpp
        ../../src/lib/marble/routing/instructions/WaypointParser.cpp
        main.cpp
)

add_definitions( -DMAKE_MARBLE_LIB )
add_executable( routing-instructions ${routing-instructions_SRC} )

if (QT4BUILD)
  target_link_libraries( routing-instructions ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} )
else()
  target_link_libraries( routing-instructions ${Qt5Core_LIBRARIES} )
endif()
