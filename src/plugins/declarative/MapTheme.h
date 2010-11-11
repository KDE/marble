//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_MAPTHEME_H
#define MARBLE_DECLARATIVE_MAPTHEME_H

#include <QtCore/QObject>

namespace Marble
{
namespace Declarative
{

/**
  * Important properties of map themes: Identifier, name, preview image.
  * This is only a data container
  */
class MapTheme : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString id READ id WRITE setId NOTIFY idChanged )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )

public:
    /** Constructor */
    MapTheme( const QString &id, const QString &name, QObject *parent = 0 );

    /** Map theme id, e.g. "earth/openstreetmap/openstreetmap.dgml" */
    QString id() const;

    /** Change the map theme id */
    void setId( const QString &id );

    /** Map theme name (GUI string) */
    QString name() const;

    /** Change the map theme name */
    void setName( const QString &name );

Q_SIGNALS:
    void idChanged();

    void nameChanged();

private:
    QString m_id;

    QString m_name;
};

}
}

#endif // MARBLE_DECLARATIVE_MAPTHEME_H
