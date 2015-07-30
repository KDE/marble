// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>

import QtQuick 2.3
import QtQuick.Dialogs 1.2

FileDialog {
    id: root
    nameFilters: [ "/" ]
    selectExisting: false
}
