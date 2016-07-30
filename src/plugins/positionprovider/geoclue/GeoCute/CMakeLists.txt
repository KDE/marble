include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(GeoCute_SRCS
    Accuracy.cpp
    PositionProvider.cpp
    Provider.cpp
    MasterClient.cpp
    SimpleDBusInterface.cpp
    VelocityProvider.cpp
    Server/AbstractProvider.cpp
    Server/PositionAdaptor.cpp
    Server/ProviderAdaptor.cpp
    Server/ProviderApplication.cpp
)

add_library(GeoCute SHARED ${GeoCute_SRCS})
