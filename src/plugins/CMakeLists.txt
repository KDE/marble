if(NOT ANDROID)
    add_subdirectory( designer )
    add_subdirectory( templates )
endif()

add_subdirectory( render )
add_subdirectory( positionprovider )
add_subdirectory( runner )
