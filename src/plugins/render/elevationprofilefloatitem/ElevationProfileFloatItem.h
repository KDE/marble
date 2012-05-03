//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef ELEVATIONPROFILEFLOATITEM_H
#define ELEVATIONPROFILEFLOATITEM_H

#include "AbstractFloatItem.h"
#include "DialogConfigurationInterface.h"

#include "ElevationProfilePlotAxis.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoGraphicsItem.h"
#include "LabelGraphicsItem.h"


namespace Ui
{
    class ElevationProfileConfigWidget;
}

namespace Marble
{

class GeoDataPlacemark;
class MarbleWidget;
class RoutingModel;

/**
 * @short The class that creates an interactive elvation profile.
 *
 */

class ElevationProfileFloatItem : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT

    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )

    MARBLE_PLUGIN( ElevationProfileFloatItem )

 public:
    ElevationProfileFloatItem();
    explicit ElevationProfileFloatItem( const MarbleModel *marbleModel );
    ~ElevationProfileFloatItem();

    virtual QStringList backendTypes() const;

    virtual qreal zValue() const; // Overriding LayerInterface to paint on top of the route

    virtual QString name() const;

    virtual QString guiString() const;

    virtual QString nameId() const;

    virtual QString version() const;

    virtual QString description() const;

    virtual QString copyrightYears() const;

    virtual QList<PluginAuthor> pluginAuthors() const;

    virtual QIcon icon() const;

    virtual void initialize();

    virtual bool isInitialized() const;

    virtual void changeViewport( ViewportParams *viewport );

    virtual void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );

    QDialog *configDialog();

 protected:
    bool eventFilter( QObject *object, QEvent *e );
    virtual void contextMenuEvent( QWidget *w, QContextMenuEvent *e );

 private Q_SLOTS:
    void updateData();
    void updateVisiblePoints();
    void forceRepaint();
    void readSettings();
    void writeSettings();
    void toggleZoomToViewport();

 Q_SIGNALS:
    void dataUpdated();

 private:
    QDialog *m_configDialog;
    Ui::ElevationProfileConfigWidget *ui_configWidget;

    int      m_leftGraphMargin;
    int      m_eleGraphWidth;
    qreal    m_viewportWidth;
    qreal    m_eleGraphHeight;
    qreal    m_shrinkFactorY;

    int      m_fontHeight;

    ElevationProfilePlotAxis m_axisX;
    ElevationProfilePlotAxis m_axisY;

    GeoDataDocument         m_markerDocument;
    GeoDataPlacemark *const m_markerPlacemark;
    int                     m_documentIndex;

    qreal    m_cursorPositionX;

    bool     m_isInitialized;

    QMenu*   m_contextMenu;

    MarbleWidget*     m_marbleWidget;
    const RoutingModel* m_routingModel;
    bool              m_routeAvailable;

    int               m_firstVisiblePoint;
    int               m_lastVisiblePoint;
    bool              m_zoomToViewport;
    QList<QPointF>    m_eleData;
    GeoDataLineString m_points;
    qreal             m_minElevation;
    qreal             m_maxElevation;
    qreal             m_gain;
    qreal             m_loss;

    QList<QPointF> calculateElevationData( const GeoDataLineString &lineString ) const;
    void calculateStatistics( const QList<QPointF> &eleData );
};


}

#endif // ELEVATIONPROFILEFLOATITEM_H
