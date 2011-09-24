PROJECT(ElevationProfileFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/elevationprofile
 ${CMAKE_CURRENT_BUILD_DIR}/src/plugins/render/elevationprofile
 ${CMAKE_BINARY_DIR}/src/plugins/render/elevationprofile
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( elevationprofile_SRCS ElevationProfileFloatItem.cpp )
set( elevationprofile_UI ElevationProfileConfigWidget.ui )

qt4_wrap_ui(elevationprofile_SRCS ${elevationprofile_UI})

marble_add_plugin( ElevationProfileFloatItem ${elevationprofile_SRCS} )
