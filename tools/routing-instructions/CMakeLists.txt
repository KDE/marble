project( RoutingInstructions )
include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( routing-instructions_SRC
        ../../src/lib/marble/routing/instructions/InstructionTransformation.cpp
        ../../src/lib/marble/routing/instructions/RoutingInstruction.cpp
        ../../src/lib/marble/routing/instructions/RoutingPoint.cpp
        ../../src/lib/marble/routing/instructions/RoutingWaypoint.cpp
        ../../src/lib/marble/routing/instructions/WaypointParser.cpp
        main.cpp
)

add_executable( routing-instructions ${routing-instructions_SRC} )

target_link_libraries( routing-instructions Qt5::Core )
