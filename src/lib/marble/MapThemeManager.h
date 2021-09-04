// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2008 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_MAPTHEMEMANAGER_H
#define MARBLE_MAPTHEMEMANAGER_H

#include <QObject>

#include "marble_export.h"

class QStandardItemModel;
class QString;
class QStringList;

namespace Marble
{

class GeoSceneDocument;
class GeoDataPhotoOverlay;
/**
 * @short The class that handles map themes that are locally available .
 *
 * This class which is able to check for maps that are locally available.
 * After parsing the data it only stores the name, description and path
 * into a QStandardItemModel.
 *
 * The MapThemeManager is not owned by the MarbleWidget/Map itself.
 * Instead it is owned by the widget or application that contains
 * MarbleWidget/Map ( usually: the ControlView convenience class )
 *
 * For convenience MarbleThemeManager provides a static helper class
 * that loads the properties of a map theme into a GeoSceneDocument
 * object.
 *
 * @see GeoSceneDocument
 */

class MARBLE_EXPORT MapThemeManager : public QObject
{
    Q_OBJECT

 public:
    explicit MapThemeManager(QObject *parent = nullptr);
    ~MapThemeManager() override;

    /**
     * @brief Returns a list of all locally available map theme IDs
     */
    QStringList mapThemeIds() const;

    /**
     * @brief Provides a model of the locally existing themes.
     *
     * This method provides a QStandardItemModel of all themes
     * that are available via MarbleDirs.
     */
    QStandardItemModel* mapThemeModel();

    /**
     * @brief Provides a model of all installed planets.
     */
    QStandardItemModel *celestialBodiesModel();

    /**
     * @brief Returns the map theme as a GeoSceneDocument object
     * @param mapThemeStringID  the string ID that refers to the map theme
     *
     * This helper method should only get used by MarbleModel to load the
     * current theme into memory or by the MapThemeManager.
     */
    static GeoSceneDocument* loadMapTheme( const QString& mapThemeStringID );

    /**
     * @brief Returns a map as a GeoSceneDocument object created from a GeoDataPhotoOverlay
     */
    static GeoSceneDocument* createMapThemeFromOverlay( const GeoDataPhotoOverlay *overlayData );

    /**
     * @brief Deletes the map theme with the specified map theme ID.
     * @param mapThemeId ID of the map theme to be deleted
     *
     * Deletion will only succeed for local map themes, that is, if the map
     * theme's directory structure resides in the user's home directory.
     */
    static void deleteMapTheme( const QString &mapThemeId );

 Q_SIGNALS:
    /**
     * @brief This signal will be emitted, when the themes change.
     */
    void themesChanged();

 private:
    Q_PRIVATE_SLOT( d, void directoryChanged( const QString& path ) )
    Q_PRIVATE_SLOT( d, void fileChanged( const QString & path ) )

    Q_DISABLE_COPY( MapThemeManager )

    class Private;
    friend class Private;
    Private * const d;
};

}

#endif
