set(marble_SRCS main.cpp)
qt5_add_resources(marble_QRCS MarbleBehaim.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (ANDROID)
    set(TARGET MarbleBehaim)

    find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS AndroidExtras)

    add_library (${TARGET} SHARED ${marble_SRCS} ${marble_QRCS})

    # while not directly linked, need to be listed here to have deployqt pick up those libs
    target_link_libraries (
        ${TARGET}
        marblewidget
        astro
        Qt6::Concurrent
        Qt6::Xml
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Network
        Qt6::Sql
        Qt6::Svg
        Qt6::Positioning
        Qt6::AndroidExtras
        Qt6::Multimedia
    )

    if (TARGET Qt6::WebEngineWidgets)
        target_link_libraries(${TARGET} Qt6::WebEngineWidgets Qt6::WebEngine)
    endif()
else()
    set(TARGET marble-behaim)
    add_executable (${TARGET} ${marble_SRCS} ${marble_QRCS})
endif()

target_link_libraries (
    ${TARGET}
    marbledeclarative
)

FILE(GLOB QML_FILES *.qml)
add_custom_target(marble-behaim_resources ALL SOURCES ${QML_FILES} package/AndroidManifest.xml)

if (ANDROID)
    install(DIRECTORY "../../../data/android/" DESTINATION "${CMAKE_INSTALL_PREFIX}/res")
    set(ABSOLUTE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
    if(NOT IS_ABSOLUTE "${ABSOLUTE_INSTALL_PATH}")
        set(ABSOLUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}")
    endif()
    get_filename_component(ABSOLUTE_INSTALL_PATH "${ABSOLUTE_INSTALL_PATH}" ABSOLUTE)
    get_filename_component(QT_ANDROID_QT_ROOT "${Qt6Core_DIR}/../../.." ABSOLUTE)
    set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
    set(ANDROID_NDK_ROOT $ENV{ANDROID_NDK_ROOT})
    set(QT_ANDROID_APP_PATH "${ABSOLUTE_INSTALL_PATH}/libs/${ANDROID_ABI}/libMarbleBehaim.so")
    set(QT_ANDROID_APP_EXTRA_LIBS "${ABSOLUTE_INSTALL_PATH}/lib/libastro.so,${ABSOLUTE_INSTALL_PATH}/lib/libmarblewidget-qt6.so,${ABSOLUTE_INSTALL_PATH}/lib/libmarbledeclarative.so")
    set(QML_ROOT_PATH "${ABSOLUTE_INSTALL_PATH}/")
    set(QT_ANDROID_APP_EXTRA_PLUGINS "${ABSOLUTE_INSTALL_PATH}//share,${ABSOLUTE_INSTALL_PATH}//lib/qml")
    set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/package")
    set(QT_ANDROID_APP_PACKAGE_NAME "org.kde.marble.behaim")
    set(QT_ANDROID_APP_NAME "Marble Globe: Behaim's Erdapfel")
    configure_file(package/deploy-behaim.json.in ${CMAKE_CURRENT_BINARY_DIR}/deploy-behaim.json @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/deploy-behaim.json DESTINATION "${CMAKE_INSTALL_PREFIX}/share")
    install(TARGETS ${TARGET} LIBRARY DESTINATION libs/${ANDROID_ABI})
    if (TARGET create-apk-MarbleBehaim)
        set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/MarbleBehaim_projects/")
        set_target_properties(create-apk-MarbleBehaim PROPERTIES ANDROID_APK_DIR "${PACKAGE_DIR}")
        add_custom_target(marblebehaim-move-metadata ALL
            COMMAND cmake -E echo "Moving MarbleBehaim shared Android resources"
            COMMAND cmake -E remove_directory "${PACKAGE_DIR}"
            COMMAND cmake -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/package" "${PACKAGE_DIR}"
            COMMAND cmake -E copy_directory "${CMAKE_SOURCE_DIR}/data/android/" "${PACKAGE_DIR}/res"
        )
        add_dependencies(create-apk-MarbleBehaim marblebehaim-move-metadata)
    endif()
else()
    if(MARBLE_INSTALL_ANDROID_APPS)
        install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
    endif()
endif()
