add_subdirectory(marble-ui)
add_subdirectory(marble-qt)
add_subdirectory(marble-mobile)

if (NOT QTONLY)
  add_subdirectory(marble-kde)
endif()

if( QT_QTDECLARATIVE_FOUND OR Qt5Quick_FOUND )
    add_subdirectory(marble-touch)
endif( QT_QTDECLARATIVE_FOUND OR Qt5Quick_FOUND )
