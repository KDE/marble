//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

// Large parts of this file are based on the file MenuItem.qml found
// in the Qt Components project with the following disclaimer:

/*
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
*/

import Qt 4.7
import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: root

    // Common API
    property string text
    signal clicked
    property alias pressed: mouseArea.pressed
    property alias checked: switchItem.checked

    // platformStyle API
    property Style platformStyle: MenuItemStyle{}

    width: parent ? parent.width: 0
    height: ( root.platformStyle.height === 0 ) ?
                root.platformStyle.topMargin + menuText.paintedHeight + root.platformStyle.bottomMargin :
                root.platformStyle.topMargin + root.platformStyle.height + root.platformStyle.bottomMargin

    BorderImage {
        id: backgroundImage
        // ToDo: remove hardcoded values
        source:  root.parent.children.length === 1 ? (parent.pressed ? "image://theme/meegotouch-list-background-pressed" : "image://theme/meegotouch-list-background")
                                                  : root.parent.children[0] === root ? (parent.pressed ? "image://theme/meegotouch-list-background-pressed-vertical-top" : "image://theme/meegotouch-list-background-vertical-top")
                                                                                    : root.parent.children[root.parent.children.length-1] === root ? (parent.pressed ? "image://theme/meegotouch-list-background-pressed-vertical-bottom" : "image://theme/meegotouch-list-background-vertical-bottom")
                                                                                                                                                  : (parent.pressed ? "image://theme/meegotouch-list-background-pressed-vertical-center" : "image://theme/meegotouch-list-background-vertical-center")
        anchors.fill : root
        border { left: 22; top: 22;
            right: 22; bottom: 22 }
    }

    Row {
        anchors.topMargin : root.platformStyle.topMargin
        anchors.bottomMargin : root.platformStyle.bottomMargin
        anchors.leftMargin : root.platformStyle.leftMargin
        anchors.rightMargin : root.platformStyle.rightMargin

        anchors.top : root.platformStyle.centered ? undefined : root.top
        anchors.bottom : root.platformStyle.centered ? undefined : root.bottom
        anchors.left : root.left
        anchors.right : root.right
        anchors.verticalCenter : root.platformStyle.centered ? parent.verticalCenter : undefined

        spacing: 10

        Switch {
            id: switchItem
            checked: true
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: menuText
            anchors.verticalCenter: parent.verticalCenter
            text: parent.parent.text
            elide: Text.ElideRight
            font.family : root.platformStyle.fontFamily
            font.pixelSize : root.platformStyle.fontPixelSize
            font.weight: root.platformStyle.fontWeight
            color: root.platformStyle.textColor
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: { if (parent.enabled) parent.clicked();}
    }

    onClicked: {
        switchItem.checked = !switchItem.checked
        if (parent) {
            parent.closeLayout();
        }
    }
}
