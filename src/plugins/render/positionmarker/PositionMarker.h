//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
//

#ifndef POSITION_MARKER_H
#define POSITION_MARKER_H

#include <QtCore/QObject>

#include "RenderPlugin.h"
#include "GeoDataCoordinates.h"
#include "PluginAboutDialog.h"

namespace Ui
{
    class PositionMarkerConfigWidget;
}

namespace Marble
{

class PositionMarker  : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( PositionMarker )
 public:
    PositionMarker ();
    ~PositionMarker ();

    QStringList renderPosition() const;

    QString renderPolicy() const;

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    QDialog *aboutDialog() const;

    QDialog *configDialog() const;

    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos, GeoSceneLayer * layer = 0 );

    void update();

    // Overriding LayerInterface to paint on top of the route
    virtual qreal zValue() const;

 public slots:
    void readSettings() const;
    void writeSettings();
    void setPosition( const GeoDataCoordinates &position );
    void chooseCustomCursor();

 private:
    Q_DISABLE_COPY( PositionMarker )

    bool           m_isInitialized;
    bool           m_useCustomCursor;
 
    mutable Ui::PositionMarkerConfigWidget *ui_configWidget;
    mutable PluginAboutDialog *m_aboutDialog;
    mutable QDialog *m_configDialog;

    ViewportParams     *m_viewport;
    GeoDataCoordinates  m_currentPosition;
    GeoDataCoordinates  m_previousPosition;

    QPolygonF           m_arrow;
    QPolygonF           m_previousArrow;
    QRegion             m_dirtyRegion;
    QPixmap             m_customCursor;

    int loadCustomCursor( const QString& filename );
};

}

#endif
