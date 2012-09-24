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
#include <QtGui/QWidget>

class QStandardItemModel;

namespace Marble
{

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
    void setMarbleWidget( MarbleWidget *widget );

 public Q_SLOTS:
    void setMapThemeId( const QString & );

    void setProjection( Projection projection );

 private:
    Q_PRIVATE_SLOT( d, void setCelestialBody( int comboIndex ) )

    /// whenever a new map gets inserted, the following slot will adapt the ListView accordingly
    Q_PRIVATE_SLOT( d, void updateMapThemeView() )

    Q_PRIVATE_SLOT( d, void projectionSelected( int projectionIndex ) )

    Q_PRIVATE_SLOT( d, void mapThemeSelected( QModelIndex index ) )
    Q_PRIVATE_SLOT( d, void mapThemeSelected( int index ) )

    Q_PRIVATE_SLOT( d, void showContextMenu( const QPoint& pos ) )
    Q_PRIVATE_SLOT( d, void deleteMap() )
    Q_PRIVATE_SLOT( d, void toggleFavorite() )

 Q_SIGNALS:
    void mapThemeIdChanged( const QString& );
    void projectionChanged( Projection );
    void showMapWizard();
    void showUploadDialog();

 private:
    Q_DISABLE_COPY( MapViewWidget )

    friend class Private;
    class Private;
    Private * const d;
};

}

#endif
