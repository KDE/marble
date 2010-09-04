find_package(libgps)
if(LIBGPS_FOUND)
     message( STATUS "Building with gpsd position provider")
     add_subdirectory( gpsd )
else(LIBGPS_FOUND)
     message( STATUS "Not building with gpsd position provider")
endif(LIBGPS_FOUND)

find_package(liblocation)
if(liblocation_FOUND)
     message( STATUS "Building with Maemo GPS support")
     add_subdirectory( maemo )
endif(liblocation_FOUND)

# experimental implementation
# add_subdirectory( geoclue )
