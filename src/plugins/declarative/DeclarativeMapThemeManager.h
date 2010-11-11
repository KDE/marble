//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef DECLARATIVE_MAPTHEMEMANAGER_H
#define DECLARATIVE_MAPTHEMEMANAGER_H

#include "MapTheme.h"
#include "MapThemeManager.h"

#include <QtDeclarative/QDeclarativeImageProvider>

namespace Marble
{
namespace Declarative
{

class MapThemeImageProvider : public QDeclarativeImageProvider
{
public:
    MapThemeImageProvider();

    QPixmap requestPixmap( const QString &id, QSize *size, const QSize &requestedSize );

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

public Q_SLOTS:
    /**
      * A list of all installed map theme ids, each entry has the form
      * "planet/themeid/themeid.dgml", e.g. "earth/bluemarble/bluemarble.dgml"
      */
    QStringList mapThemeIds() const;

    /**
      * A list of all installed map themes. Each list item is an instance
      * of a MapTheme. This list is useful to feed to qml views.
      */
    QList<QObject*> mapThemes();

private:
    /** Marble map theme manager doing the real work */
    Marble::MapThemeManager m_mapThemeManager;
};

} // namespace Declarative
} // namespace Marble

#endif // DECLARATIVE_MAPTHEMEMANAGER_H
