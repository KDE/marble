// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#ifndef MARBLE_MARBLEDIRS_H
#define MARBLE_MARBLEDIRS_H

#include "marble_export.h"

#include <QDir>
#include <QString>
#include <QStringList>

namespace Marble
{

/**
 * @short A class that manages data look-up for Marble.
 *
 * The class basically does for Marble what KStandardDirs did for KDE4.
 * Given that the MarbleWidget is a Qt6-only library and given that it
 * comes with its own model and data we need this class.
 *
 * The class needs to respect the requirements of the different platforms,
 * so to avoid very different implementations for each platform we
 * specify how data should get looked up:
 *
 * Generally there are three places from which Marble will draw its data
 * from: "cachePath", "localPath" and "systemPath".
 *
 * Map tiles are generated or downloaded into the cachePath.  Map tile
 * data should be looked up in the cachePath first, and then if it is
 * not present there then in the localPath and finally the systemPath.
 * Other map data - DGML definition files, level zero map tiles, preview
 * and legend images - are not stored in the cache but in either the
 * systemPath for maps provided as standard with Marble, or the localPath
 * for maps installed by the user or created via the map wizard.
 *
 * cachePath:
 * The place for cachePath should match space that is fully accessible to
 * the user, but which may be cleared at any time. On Unix-like platforms
 * this is specified by the XDG_CACHE_HOME environment variable, or if that
 * is not set then the default is ".cache/marble" in the user's home directory.
 *
 * localPath:
 * The place for localPath should match space that is fully accessible to
 * the user, but is permanent. On Unix-like platforms this is specified by
 * the XDG_DATA_HOME environment variable, or if that is not set then the
 * default is ".local/share/marble" in the user's home directory.
 *
 * systemPath:
 * Ideally the systemPath should match the place where CMake installed the
 * data for Marble. However this doesn't work for all platforms:
 *
 * - For Linux and Mac non-bundle deployment the location can be
 *   chosen using the CMake MARBLE_DATA_PATH option at compile time.
 * - For Mac bundle deployment the location inside the bundle gets
 *   chosen as the default location.
 * - For Windows a path relative to the application binary can be chosen
 *   as this should usually work without problems.
 *
 * To allow kiosk-like setups and for custom setups in general
 * it should be possible to change the place of the systemPath at runtime.
 * Therefore we introduce a global variable "MarbleDataPath" in the
 * MarbleDirs.h source code.
 * Initially MarbleDataPath is empty. The systemPath will point to
 * MarbleDataPath as soon as it gets changed to a valid non-empty path. So
 * as soon as MarbleDataPath contains a valid path the path specified by
 * cmake will get ignored.
 *
 * (Possible future extension: if the MarbleDataPath contains several
 * valid paths separated by a colon then each of these paths should be
 * used for look up in the same order as for the KDE kiosk framework.)
 *
 * It's the duty of each application that uses the MarbleWidget to retrieve
 * the value of the MarbleDataPath from the MarbleWidget and to save it
 * in its settings and restore it on start-up of the application.
 *
 */

// TODO: no private data, this can be just a namespace
class MARBLE_EXPORT MarbleDirs
{
public:
    MarbleDirs();

    // TODO: rename this to "dataFilePath" to make its function explicit
    static QString path(const QString &relativePath);

    static QString cacheFilePath(const QString &relativePath);

    // TODO: rename this to "pluginFilePath" for consistency
    static QString pluginPath(const QString &relativePath);

    static QStringList entryList(const QString &relativePath, QDir::Filters filters = QDir::NoFilter);

    static QStringList pluginEntryList(const QString &relativePath, QDir::Filters filters = QDir::NoFilter);

    static QString systemPath();

    static QString pluginSystemPath();

    static QString localPath();

    static QString cachePath();

    static QStringList oldLocalPaths();

    static QString pluginLocalPath();

    static QString marbleDataPath();

    static QString marblePluginPath();

    static void setMarbleDataPath(const QString &adaptedPath);

    static void setMarblePluginPath(const QString &adaptedPath);

    static void debug();

private:
    Q_DISABLE_COPY(MarbleDirs)
    class Private;
    Private *const d;
};

}

#endif
