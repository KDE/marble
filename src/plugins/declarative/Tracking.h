//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_TRACKING_H
#define MARBLE_DECLARATIVE_TRACKING_H

#include "PositionSource.h"

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>

namespace Marble
{

class MarbleWidget;

namespace Declarative
{

class Tracking : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool showPosition READ showPosition WRITE setShowPosition NOTIFY showPositionChanged )
    Q_PROPERTY( bool showTrack READ showTrack WRITE setShowTrack NOTIFY showTrackChanged )
    Q_PROPERTY( Marble::Declarative::PositionSource* positionSource READ positionSource WRITE setPositionSource NOTIFY positionSourceChanged )
    Q_PROPERTY( QObject* positionMarker READ positionMarker WRITE setPositionMarker NOTIFY positionMarkerChanged )

public:
    explicit Tracking( QObject* parent = 0);

    bool showPosition() const;

    void setShowPosition( bool show );

    bool showTrack() const;

    void setShowTrack( bool show );

    Marble::Declarative::PositionSource* positionSource();

    void setPositionSource( Marble::Declarative::PositionSource* source );

    QObject* positionMarker();

    void setPositionMarker( QObject* marker );

    void setMarbleWidget( Marble::MarbleWidget* widget );

Q_SIGNALS:
    void showPositionChanged();

    void showTrackChanged();

    void positionSourceChanged();

    void positionMarkerChanged();

private Q_SLOTS:
    void updatePositionMarker();

private:
    void setShowPositionMarkerPlugin( bool visible );

    bool m_showPosition;

    bool m_showTrack;

    Marble::Declarative::PositionSource* m_positionSource;

    QObject* m_positionMarker;

    Marble::MarbleWidget* m_marbleWidget;
};

}

}

#endif
