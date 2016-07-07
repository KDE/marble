//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_MAPVIEWWIDGET_H
#define MARBLE_MAPVIEWWIDGET_H

// Marble
#include "MarbleGlobal.h"
#include "marble_export.h"

// Qt
#include <QWidget>

namespace Marble
{

class MapThemeManager;
class MarbleWidget;

class MARBLE_EXPORT MapViewWidget : public QWidget
{
    Q_OBJECT
 public:
    explicit MapViewWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~MapViewWidget();

    /**
     * @brief Set a MarbleWidget associated to this widget.
     * @param widget  the MarbleWidget to be set.
     */
    void setMarbleWidget( MarbleWidget *widget, MapThemeManager *mapThemeManager );

protected:
    void resizeEvent(QResizeEvent *event);

 public Q_SLOTS:
    void setMapThemeId( const QString & );
    void setProjection( Projection projection );

private Q_SLOTS:
    void globeViewRequested();
    void mercatorViewRequested();
    void flatViewRequested();
    void gnomonicViewRequested();
    void stereographicViewRequested();
    void lambertAzimuthalViewRequested();
    void azimuthalEquidistantViewRequested(); 
    void verticalPerspectiveViewRequested();

 private:
    Q_PRIVATE_SLOT( d, void celestialBodySelected( int comboIndex ) )

    Q_PRIVATE_SLOT( d, void projectionSelected( int projectionIndex ) )

    Q_PRIVATE_SLOT( d, void mapThemeSelected( QModelIndex index ) )
    Q_PRIVATE_SLOT( d, void mapThemeSelected( int index ) )

    Q_PRIVATE_SLOT( d, void showContextMenu( const QPoint& pos ) )
    Q_PRIVATE_SLOT( d, void deleteMap() )
    Q_PRIVATE_SLOT( d, void toggleFavorite() )
    Q_PRIVATE_SLOT( d, void toggleIconSize() )

 Q_SIGNALS:
    void celestialBodyChanged( const QString& );
    void mapThemeIdChanged( const QString& );
    void projectionChanged( Projection );
    void showMapWizard();
    void showUploadDialog();
    void mapThemeDeleted();

 private:
    Q_DISABLE_COPY( MapViewWidget )

    friend class Private;
    class Private;
    Private * const d;
};

}

#endif
