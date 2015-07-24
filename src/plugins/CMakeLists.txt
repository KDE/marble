#FIXME: Port to Android
if(NOT ANDROID)
    add_subdirectory( designer )
    add_subdirectory( declarative )
    add_subdirectory( render )
    add_subdirectory( templates )
    add_subdirectory( positionprovider )
endif()

add_subdirectory( runner )
