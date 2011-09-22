//
// This file is part of the Marble Virtual Globe.
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
#include "routing/AdjustNavigation.h"

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
    explicit CurrentLocationWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~CurrentLocationWidget();

    /**
     * @brief Set a MarbleWidget associated to this widget.
     * @param widget  the MarbleWidget to be set.
     */
    void setMarbleWidget( MarbleWidget *widget );

    AdjustNavigation::CenterMode recenterMode() const;

    bool autoZoom() const;

    bool trackVisible() const;

    QString lastOpenPath() const;

    QString lastSavePath() const;

 public Q_SLOTS:
    void receiveGpsCoordinates( const GeoDataCoordinates& in, qreal speed );

     /// Slot that decides whether recentering should be done
    void setRecenterMode( int recenterMode );

    /// Slot for Auto Zooming while navigating
    void setAutoZoom( bool activate );

    void setTrackVisible( bool visible );

    void setLastOpenPath( const QString &path );

    void setLastSavePath( const QString &path );

 private:
    Q_DISABLE_COPY( CurrentLocationWidget )

    CurrentLocationWidgetPrivate * const d;

    Q_PRIVATE_SLOT( d, void adjustPositionTrackingStatus( PositionProviderStatus status ) )
    Q_PRIVATE_SLOT( d, void changePositionProvider( const QString &provider ) )
    Q_PRIVATE_SLOT( d, void centerOnCurrentLocation() )

    /**
     * @brief Slot for setting re-center combobox if re-centering is enabled other than from CurrentLocationWidget
     * @see RoutingPlugin
     */
     Q_PRIVATE_SLOT( d, void updateRecenterComboBox( AdjustNavigation::CenterMode centerMode ) )

    /**
     * @brief Slot for toggling auto zoom checkbox if auto zooming is enabled other than from CurrentLocationWidget
     * @see RoutingPlugin
     */
     Q_PRIVATE_SLOT( d, void updateAutoZoomCheckBox( bool autoZoom ) )

     Q_PRIVATE_SLOT( d, void updateActivePositionProvider( PositionProviderPlugin* ) )

     Q_PRIVATE_SLOT( d, void saveTrack() )
     Q_PRIVATE_SLOT( d, void openTrack() )
     Q_PRIVATE_SLOT( d, void clearTrack() )
};

}

#endif
