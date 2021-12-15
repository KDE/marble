// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_CURRENTLOCATIONWIDGET_H
#define MARBLE_CURRENTLOCATIONWIDGET_H

// Marble
#include "marble_export.h"
#include "AutoNavigation.h"

// Qt
#include <QWidget>

namespace Marble
{

class CurrentLocationWidgetPrivate;

class GeoDataCoordinates;
class MarbleWidget;
class PositionProviderPlugin;

class MARBLE_EXPORT CurrentLocationWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit CurrentLocationWidget( QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() );
    ~CurrentLocationWidget() override;

    /**
     * @brief Set a MarbleWidget associated to this widget.
     * @param widget  the MarbleWidget to be set.
     */
    void setMarbleWidget( MarbleWidget *widget );

    AutoNavigation::CenterMode recenterMode() const;

    bool autoZoom() const;

    bool trackVisible() const;

    QString lastOpenPath() const;

    QString lastSavePath() const;

 public Q_SLOTS:
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

    Q_PRIVATE_SLOT( d, void receiveGpsCoordinates( const GeoDataCoordinates &in, qreal speed ) )
    Q_PRIVATE_SLOT( d, void adjustPositionTrackingStatus( PositionProviderStatus status ) )
    Q_PRIVATE_SLOT( d, void changePositionProvider( const QString &provider ) )
    Q_PRIVATE_SLOT( d, void centerOnCurrentLocation() )

    Q_PRIVATE_SLOT( d, void trackPlacemark() )

    /**
     * @brief Slot for setting re-center combobox if re-centering is enabled other than from CurrentLocationWidget
     * @see RoutingPlugin
     */
     Q_PRIVATE_SLOT( d, void updateRecenterComboBox( AutoNavigation::CenterMode centerMode ) )

    /**
     * @brief Slot for toggling auto zoom checkbox if auto zooming is enabled other than from CurrentLocationWidget
     * @see RoutingPlugin
     */
     Q_PRIVATE_SLOT( d, void updateAutoZoomCheckBox( bool autoZoom ) )

     Q_PRIVATE_SLOT( d, void updateActivePositionProvider( PositionProviderPlugin* ) )

     Q_PRIVATE_SLOT( d, void updateGuidanceMode() )

     Q_PRIVATE_SLOT( d, void saveTrack() )
     Q_PRIVATE_SLOT( d, void openTrack() )
     Q_PRIVATE_SLOT( d, void clearTrack() )
};

}

#endif
