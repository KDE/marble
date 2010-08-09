//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_CURRENTLOCATIONWIDGET_H
#define MARBLE_CURRENTLOCATIONWIDGET_H

// Marble
#include "marble_export.h"
#include "PositionProviderPlugin.h"

// Qt
#include <QtGui/QWidget>

namespace Marble
{

class CurrentLocationWidgetPrivate;

class GeoDataCoordinates;
class MarbleWidget;

class MARBLE_EXPORT CurrentLocationWidget : public QWidget
{
    Q_OBJECT

 public:
    CurrentLocationWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~CurrentLocationWidget();

    /**
     * @brief Set a MarbleWidget associated to this widget.
     * @param widget  the MarbleWidget to be set.
     */
    void setMarbleWidget( MarbleWidget *widget );

 public Q_SLOTS:
    void receiveGpsCoordinates( const GeoDataCoordinates& in, qreal speed );

     /// Slot that decides whether recentering should be done
    void setRecenter( int activate );

    ///Slot for Auto Zooming while navigating
    void setAutoZoom( bool activate );

 private Q_SLOTS:
    void adjustPositionTrackingStatus( PositionProviderStatus status );
    void changePositionProvider( const QString &provider );
    void centerOnCurrentLocation();

 private:
    Q_DISABLE_COPY( CurrentLocationWidget )

    CurrentLocationWidgetPrivate * const d;
};

}

#endif
