//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef DECLARATIVE_MAPTHEMEMANAGER_H
#define DECLARATIVE_MAPTHEMEMANAGER_H

#include "MapThemeManager.h"

#include <QQuickImageProvider>

class MapThemeImageProvider : public QQuickImageProvider
{
public:
    MapThemeImageProvider();

    QPixmap requestPixmap( const QString &id, QSize *size, const QSize &requestedSize ) override;

private:
    /** @todo FIXME It is a bit ugly to keep a separate object that loads all themes again
      * just to provide their images to QML. The API change before Qt 4.7 made this necessary
      * however (it's not possible anymore to assign a QPixmap property to QML images) */
    Marble::MapThemeManager m_mapThemeManager;
};

/**
  * Provides access to all map themes installed locally
  */
class MapThemeManager : public QObject
{
    Q_OBJECT

public:
    /** Constructor. Map themes are loaded later on demand. */
    explicit MapThemeManager( QObject *parent = 0 );

    /**
      * A list of all installed map theme ids, each entry has the form
      * "planet/themeid/themeid.dgml", e.g. "earth/bluemarble/bluemarble.dgml"
      */
    QStringList mapThemeIds() const;

private:
    /** Marble map theme manager doing the real work */
    Marble::MapThemeManager m_mapThemeManager;
};

#endif // DECLARATIVE_MAPTHEMEMANAGER_H
