#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

SET( graphicsview_HDRS 
        graphicsview/MarbleGraphicsItem.h
        graphicsview/GeoGraphicsItem.h
        graphicsview/ScreenGraphicsItem.h
        graphicsview/FrameGraphicsItem.h
        graphicsview/LabelGraphicsItem.h
   )

SET( graphicsview_SRCS
        graphicsview/MarbleGraphicsItem.cpp
        graphicsview/GeoGraphicsItem.cpp
        graphicsview/ScreenGraphicsItem.cpp
        graphicsview/FrameGraphicsItem.cpp
        graphicsview/LabelGraphicsItem.cpp
   )
