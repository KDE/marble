//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_MAPTHEMEMANAGER_H
#define MARBLE_MAPTHEMEMANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "marble_export.h"

class QStandardItem;
class QStandardItemModel;
class QString;

namespace Marble
{

class GeoSceneDocument;
class MapThemeManagerPrivate;

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
    explicit MapThemeManager(QObject *parent = 0);
    ~MapThemeManager();

    /**
     * @brief Returns a list of all locally available map themes
     */
    QList<GeoSceneDocument const*> mapThemes() const;

    /**
     * @brief Provides a model of the locally existing themes. 
     *
     * This method provides a QStandardItemModel of all themes  
     * that are available via MarbleDirs.
     */
    QStandardItemModel* mapThemeModel();

    /**
     * @brief Returns the map theme as a GeoSceneDocument object
     * @param mapThemeStringID  the string ID that refers to the map theme
     *
     * This helper method should only get used by MarbleModel to load the
     * current theme into memory or by the MapThemeManager.
     */
    
    static GeoSceneDocument* loadMapTheme( const QString& mapThemeStringID );

 public Q_SLOTS:
    /**
     * @brief Updates the map theme model on request. 
     *
     * This method should usually get invoked on startup or 
     * by a QFileSystemWatcher instance.
     */
    void updateMapThemeModel();

    void directoryChanged( const QString& path );
    void fileChanged( const QString & path );

 Q_SIGNALS:
    /**
     * @brief This signal will be emitted, when the themes change.
     */
    void themesChanged();

 private:
    Q_DISABLE_COPY( MapThemeManager )

    /**
     * @brief Adds directory paths and .dgml file paths to the given QStringList.
     */
    static void addMapThemePaths( const QString& mapPathName, QStringList& result );

    /**
     * @brief Helper method for findMapThemes(). Searches for .dgml files below
     *        given directory path.
     */
    static QStringList findMapThemes( const QString& basePath );

    /**
     * @brief Searches for .dgml files below local and system map directory.
     */
    static QStringList findMapThemes();

    static GeoSceneDocument* loadMapThemeFile( const QString& mapThemePath );

    /**
     * @brief Returns all directory paths and .dgml file paths below local and
     *        system map directory.
     */
    static QStringList pathsToWatch();

    /**
     * @brief Helper method for updateMapThemeModel().
     */
    QList<QStandardItem *> createMapThemeRow( const QString& mapThemeID );

    /**
     * @brief Initialization.
     *
     * This method allows to provide a delayed initialization 
     * once the model is requested.
     */
    void initialize();
    
    void initFileSystemWatcher();

    MapThemeManagerPrivate * const d;
};

}

#endif
