//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn     <tackat@kde.org>
// Copyright 2007      Inge Wallin      <ingwa@kde.org>
// Copyright 2011      Michael Henning  <mikehenning@eclipse.net>
//

//
// MeasureToolPlugin enables Marble to set and display measure points
//

#ifndef MARBLE_MEASURETOOLPLUGIN_H
#define MARBLE_MEASURETOOLPLUGIN_H

#include "DialogConfigurationInterface.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "RenderPlugin.h"
#include "MarbleWidget.h"

#include <QFont>
#include <QPen>
#include <QAction>
#include <QPixmap>

namespace Marble
{

class MeasureConfigDialog;
class MeasureToolPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MeasureToolPlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( MeasureToolPlugin )

 public:
    explicit MeasureToolPlugin( const MarbleModel *marbleModel = 0 );

    enum PaintMode {
        Polygon = 0,
        Circular
    };

    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString guiString() const;
    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    QDialog *configDialog();
    QHash<QString,QVariant> settings() const;
    void setSettings( const QHash<QString,QVariant> &settings );

 Q_SIGNALS:
    void  numberOfMeasurePointsChanged( int newNumber );

 public Q_SLOTS:
    bool  eventFilter( QObject *object, QEvent *event );

 private:
    void  drawMeasurePoints( GeoPainter *painter );
    void  drawInfobox( GeoPainter *painter ) const;
    void  drawSegments( GeoPainter *painter );
    void  addContextItems();
    void  removeContextItems();

 private Q_SLOTS:
    void  setNumberOfMeasurePoints( int number );
    void  addMeasurePointEvent();

    void  addMeasurePoint( qreal lon, qreal lat );
    void  removeLastMeasurePoint();
    void  removeMeasurePoints();

    void writeSettings();

 private:
    Q_DISABLE_COPY( MeasureToolPlugin )

    QString meterToPreferredUnit(qreal meters, bool isSquare = false) const;

    // The line strings in the distance path.
    GeoDataLineString m_measureLineString;
    GeoDataLatLonAltBox m_latLonAltBox;

    QPixmap m_mark;
    QFont   m_font_regular;
    int     m_fontascent;

    QPen    m_pen;

    QAction *m_addMeasurePointAction;
    QAction *m_removeLastMeasurePointAction;
    QAction *m_removeMeasurePointsAction;
    QAction *m_separator;

    MarbleWidget* m_marbleWidget;

    MeasureConfigDialog *m_configDialog;

    bool m_showDistanceLabel;
    bool m_showBearingLabel;
    bool m_showBearingChangeLabel;

    bool m_showPolygonArea;
    bool m_showCircularArea;
    bool m_showRadius;
    bool m_showPerimeter;
    bool m_showCircumference;

    qreal m_totalDistance;
    qreal m_polygonArea;
    qreal m_circularArea;
    qreal m_radius;
    qreal m_perimeter;
    qreal m_circumference;

    PaintMode m_paintMode;
};

}

#endif // MARBLE_MEASURETOOLPLUGIN_H
