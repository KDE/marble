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
#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtGui/QColor>
#include <QtGui/QAbstractButton>

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

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( QHash<QString,QVariant> settings );


 public slots:
    void readSettings() const;
    void writeSettings();
    void updateSettings();
    void setPosition( const GeoDataCoordinates &position );
    void chooseCustomCursor();
    void chooseColor();
    void resizeCursor( int step );

 private:
    Q_DISABLE_COPY( PositionMarker )

    bool           m_isInitialized;
    bool           m_useCustomCursor;
    
    QString m_defaultCursorPath;
    ViewportParams     *m_viewport;
    GeoDataCoordinates  m_currentPosition;
    GeoDataCoordinates  m_previousPosition;
    
    mutable Ui::PositionMarkerConfigWidget *ui_configWidget;
    mutable PluginAboutDialog *m_aboutDialog;
    mutable QDialog *m_configDialog;
    mutable QString m_cursorPath;

    QPolygonF           m_arrow;
    QPolygonF           m_previousArrow;
    QRegion             m_dirtyRegion;
    QPixmap             m_customCursor;
    QPixmap             m_defaultCursor;
    QHash<QString,QVariant> m_settings;
    float               m_cursorSize;
    QColor              m_acColor;
    QColor              m_trailColor;
    qreal               m_heading;
    QVector<GeoDataCoordinates> m_trail;
    static const int    sm_numTrailPoints = 5;
    int                 m_visibleTrailPoints;
    bool                m_showTrail;

    static const int sm_defaultSizeStep;
    static const int sm_numResizeSteps;
    static const float sm_resizeSteps[];

    void loadCustomCursor( const QString& filename, bool useCursor );
    void loadDefaultCursor();

 private slots:
    void evaluateClickedButton( QAbstractButton *button );
};

}

#endif
