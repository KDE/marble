include_directories(
    ${QT_INCLUDES}
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

qt4_automoc(${GeoCute_SRCS})

add_library(GeoCute SHARED ${GeoCute_SRCS})

target_link_libraries(GeoCute
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
)

set_target_properties(GeoCute PROPERTIES DEFINE_SYMBOL MAKE_GEOCUTE_LIB)
