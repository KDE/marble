project(marble-network-kio)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories(
  ${QT_INCLUDE_DIR}
  ${KDE4_INCLUDES}
)

set(networkkio_SRCS
  KIONetworkPlugin.cpp
)

marble_add_plugin(MarbleNetworkKIO ${networkkio_SRCS})
