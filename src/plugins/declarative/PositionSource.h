//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_POSITIONSOURCE_H
#define MARBLE_DECLARATIVE_POSITIONSOURCE_H

#include "Coordinate.h"

#include <QObject>
#if QT_VERSION < 0x050000
  #include <QtDeclarative/qdeclarative.h>
#else
  #include <QtQml/qqml.h>
#endif

class MarbleWidget;

class PositionSource : public QObject
{
    Q_OBJECT

    Q_PROPERTY( MarbleWidget* map READ map WRITE setMap NOTIFY mapChanged )
    Q_PROPERTY( bool active READ active WRITE setActive NOTIFY activeChanged )
    Q_PROPERTY( QString source READ source WRITE setSource NOTIFY sourceChanged )
    Q_PROPERTY( bool hasPosition READ hasPosition NOTIFY hasPositionChanged )
    Q_PROPERTY( Coordinate* position READ position NOTIFY positionChanged )
    Q_PROPERTY( qreal speed READ speed NOTIFY speedChanged )

public:
    explicit PositionSource( QObject* parent = 0);

    bool active() const;

    void setActive( bool active );

    QString source() const;

    void setSource( const QString &source );

    bool hasPosition() const;

    Coordinate* position();

    MarbleWidget *map();

    void setMap( MarbleWidget *map );

    qreal speed() const;

Q_SIGNALS:
    void mapChanged();

    void activeChanged();

    void sourceChanged();

    void hasPositionChanged();

    void positionChanged();

    void speedChanged();

private Q_SLOTS:
    void updatePosition();

private:
    void start();

    bool m_active;

    QString m_source;

    bool m_hasPosition;

    Coordinate m_position;

    QPointer<MarbleWidget> m_marbleWidget;

    qreal m_speed;
};

#endif
