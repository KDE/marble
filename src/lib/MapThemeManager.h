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

class MapThemeManager : public QObject
{
    Q_OBJECT

public:
    MapThemeManager(QObject *parent = 0);
    ~MapThemeManager();
/*
    int open( const QString& path );

    int maxTileLevel()      const { return m_maxtilelevel;}

    static QStringList findMapThemes( const QString& );
    static QStandardItemModel* mapThemeModel( const QStringList& stringlist );
*/
};


#endif // MAPTHEMEMANAGER_H
