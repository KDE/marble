if(NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    add_subdirectory( designer )
    add_subdirectory( templates )
endif()

add_subdirectory( render )
add_subdirectory( positionprovider )
add_subdirectory( runner )
