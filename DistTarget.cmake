################################################################
# dist target
# from cmake wiki, feel free to modify
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Marble")
SET(CPACK_PACKAGE_VENDOR "Torsten Rahn")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
#SET(CPACK_INSTALL_CMAKE_PROJECTS "marble")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Marble ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

IF(WIN32 AND NOT UNIX)
  SET(CPACK_GENERATOR NSIS)     # can be NSIS, STGZ, TBZ2, TGZ, TZ and ZIP
  SET(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backslashes.
  # SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/install_win32\\\\sim-window-small.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "marble.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\kde-apps.org\\\\content\\\\show.php\\\\Marble+-+Desktop+Globe?content=55105")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\kde-apps.org\\\\content\\\\show.php\\\\Marble+-+Desktop+Globe?content=55105")
  SET(CPACK_NSIS_CONTACT "http:\\\\\\\\kde-apps.org\\\\usermanager\\\\search.php?username=tackat")

  SET(CPACK_SOURCE_GENERATOR "NSIS")
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_GENERATOR TBZ2)     # can be STGZ, TBZ2, TGZ, TZ and ZIP

  SET(CPACK_SOURCE_GENERATOR "TBZ2;TGZ")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.svn/")
SET(CPACK_PACKAGE_EXECUTABLES "marble" "Marble 0.3")

INCLUDE(CPack)
################################################################
