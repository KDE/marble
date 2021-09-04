/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PLUGIN_H
#define PLUGIN_H

#include <MarbleDeclarativePlugin.h>

/**
 * For now installing this separate Marble QtQuick plugin under a private namespace,
 * org.kde.marble.private.plasma (see qmldir)
 * until we have sorted out a general public Marble QtQuick plugin API
 * and libmarbledeclarative is turned back into a plugin instead of shared library
 */
class MarbleQuickPlugin : public MarbleDeclarativePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
};

#endif // PLUGIN_H
