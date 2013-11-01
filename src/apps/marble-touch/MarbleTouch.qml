// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0

Item {
    property string version: changelogModel.get(0).version
    property string status: "experimental release"
    property alias changelog: changelogModel

    ListModel {
        id: changelogModel

        ListElement {
            version: "1.3.3"
            summary: "Bookmark support, OSRM routing backend, download tiles in the visible area."
        }
        ListElement {
            version: "1.3.2"
            summary: "Pinch zoom, MapQuest routing backend, download tiles along the route, more voice navigation announcements."
        }
        ListElement {
            version: "1.3.1"
            summary: "First public release."
        }
    }
}
