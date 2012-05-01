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

#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"

namespace Ui
{
    class PositionMarkerConfigWidget;
}

namespace Marble
{

class PositionMarker  : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( PositionMarker )
 public:
    PositionMarker ();
    explicit PositionMarker( const MarbleModel *marbleModel );
    ~PositionMarker ();

    QStringList renderPosition() const;

    QString renderPolicy() const;

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon () const;

    QDialog *configDialog();

    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos, GeoSceneLayer * layer = 0 );

    // Overriding LayerInterface to paint on top of the route
    virtual qreal zValue() const;

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( const QHash<QString,QVariant> &settings );


 public slots:
    void readSettings();
    void writeSettings();

    void setPosition( const GeoDataCoordinates &position );
    void chooseCustomCursor();
    void chooseColor();
    void resizeCursor( int step );

 private:
    Q_DISABLE_COPY( PositionMarker )

    void update( const ViewportParams *viewport, MapQuality mapQuality );
    void loadCustomCursor( const QString& filename, bool useCursor );
    void loadDefaultCursor();

    bool           m_isInitialized;
    bool           m_useCustomCursor;

    const QString m_defaultCursorPath;
    GeoDataLatLonAltBox m_lastBoundingBox;
    GeoDataCoordinates  m_currentPosition;
    GeoDataCoordinates  m_previousPosition;
    
    Ui::PositionMarkerConfigWidget *ui_configWidget;
    QDialog *m_configDialog;
    QString m_cursorPath;

    QPolygonF           m_arrow;
    QPolygonF           m_previousArrow;
    QRegion             m_dirtyRegion;
    QPixmap             m_customCursor;
    QPixmap             m_customCursorTransformed;
    QPixmap             m_defaultCursor;
    float               m_cursorSize;
    QColor              m_accuracyColor;
    QColor              m_trailColor;
    qreal               m_heading;
    QVector<GeoDataCoordinates> m_trail;
    static const int    sm_numTrailPoints = 6;
    bool                m_showTrail;

    static const int sm_defaultSizeStep;
    static const int sm_numResizeSteps;
    static const float sm_resizeSteps[];
};

}

#endif
