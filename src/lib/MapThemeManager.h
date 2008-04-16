//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef MAPTHEMEMANAGER_H
#define MAPTHEMEMANAGER_H


#include <QtCore/QObject>

#include "marble_export.h"

class GeoSceneDocument;


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

class MapThemeManager : public QObject
{
    Q_OBJECT

public:
    MapThemeManager(QObject *parent = 0);
    ~MapThemeManager();

    /**
     * @brief Returns the map theme as a GeoSceneDocument object
     * @param mapThemeStringID  the string ID that refers to the map theme
     *
     * This helper method should only get used by MarbleModel to load the
     * current theme into memory or by the MapThemeManager.
     */
    
    static GeoSceneDocument* loadMapTheme( const QString& mapThemeStringID );
/*
    int open( const QString& path );

    int maxTileLevel()      const { return m_maxtilelevel;}

    static QStringList findMapThemes( const QString& );
    static QStandardItemModel* mapThemeModel( const QStringList& stringlist );
*/
};


#endif // MAPTHEMEMANAGER_H
