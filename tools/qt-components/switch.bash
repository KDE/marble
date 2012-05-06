#!/bin/bash

set -e

function usage()
{
  echo "Usage: switch <to>"
  echo -e "\t<to> can be either meego or plasma"
}

if [[ "${1}" == "meego" ]]
then
  IMPORT_REPLACEMENT="s/org.kde.plasma.components 0.1/com.nokia.meego 1.0/g"
  PATH_REPLACEMENT="s@plasma/@harmattan/@g"
  CMAKE_REPLACEMENT="s/SET(MARBLE_PLASMA_COMPONENTS TRUE)/SET(MARBLE_PLASMA_COMPONENTS FALSE)/"
elif [[ "${1}" == "plasma" ]]
then
  IMPORT_REPLACEMENT="s/com.nokia.meego 1.0/org.kde.plasma.components 0.1/g"
  PATH_REPLACEMENT="s@harmattan/@plasma/@g"
  CMAKE_REPLACEMENT="s/SET(MARBLE_PLASMA_COMPONENTS FALSE)/SET(MARBLE_PLASMA_COMPONENTS TRUE)/"
else
  usage
  exit 2
fi

BASE="$(dirname ${0})/../.."
PLUGIN="${BASE}/src/plugins/qt-components"
APP="${BASE}/src/qt-components/marble-touch"

test -e "${PLUGIN}/qmldir" || { echo "Unexpected path or file missing"; usage; exit 1; }
test -f "${APP}/meego.qrc"

# Platform specific .qml files in the application
sed -i "${PATH_REPLACEMENT}" "${APP}/meego.qrc"

# Platform specific .qml files in the plugin
find "${PLUGIN}" "${APP}" -name "*.qml" -exec sed -i "${IMPORT_REPLACEMENT}" {} \;

# Platform specific build option in the plugin
sed -i "${CMAKE_REPLACEMENT}" "${PLUGIN}/CMakeLists.txt"
