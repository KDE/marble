/*
 * Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
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
