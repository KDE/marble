#!/bin/bash

set -e

which wget > /dev/null

ROOT_DIR="$(dirname $0)/../.."
LANG_DIR="${ROOT_DIR}/data/lang"
MAP_DIR="${ROOT_DIR}/data/maps"
TR_TOOL="${ROOT_DIR}/tools/translations/marble_qt_i18n.bash"

test -e "${LANG_DIR}" || { echo "Creating data/lang directory."; mkdir "${LANG_DIR}"; }
test -d "${LANG_DIR}"
test -e "${MAP_DIR}"
test -d "${MAP_DIR}"

test -e "${TR_TOOL}"
test -f "${TR_TOOL}"
test -x "${TR_TOOL}"

for map in venus mars hikebikemap osmarender public-transport
do
  echo "Installing ${map} map theme to $(readlink -f ${MAP_DIR})"
  wget -q "http://files.kde.org/marble/newstuff/maps/${map}-marblemap.tar.gz" -O - | tar -C "${MAP_DIR}" -xzf -
done

echo "Generating translations in $(readlink -f ${LANG_DIR})"
"${TR_TOOL}" "${LANG_DIR}"
