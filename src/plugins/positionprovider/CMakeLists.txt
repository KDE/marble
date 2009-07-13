find_package(libgps)

if(libgps_FOUND)

     message( STATUS "Building with gpsd position provider")
     add_subdirectory( gpsd )

else(libgps_FOUND)

     message( STATUS "Not building with gpsd position provider")

endif(libgps_FOUND)

# experimental implementation
# add_subdirectory( geoclue )
