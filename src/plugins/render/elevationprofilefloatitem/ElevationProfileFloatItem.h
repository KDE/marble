//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013      Roman Karlstetter <roman.karlstetter@googlemail.com>
//

#ifndef ELEVATIONPROFILEFLOATITEM_H
#define ELEVATIONPROFILEFLOATITEM_H

#include "AbstractFloatItem.h"
#include "DialogConfigurationInterface.h"

#include "ElevationProfileDataSource.h"
#include "ElevationProfilePlotAxis.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"

namespace Ui
{
    class ElevationProfileConfigWidget;
}

namespace Marble
{

class ElevationProfileContextMenu;
class ElevationProfileDataSource;
class ElevationProfileTrackDataSource;
class ElevationProfileRouteDataSource;
class GeoDataPlacemark;
class MarbleWidget;

/**
 * @short The class that creates an interactive elevation profile.
 *
 */

class ElevationProfileFloatItem : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.ElevationProfileFloatItem")

    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )

    MARBLE_PLUGIN( ElevationProfileFloatItem )

 public:
    explicit ElevationProfileFloatItem( const MarbleModel *marbleModel = nullptr );
    ~ElevationProfileFloatItem() override;

    QStringList backendTypes() const override;

    qreal zValue() const override; // Overriding LayerInterface to paint on top of the route

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    void initialize() override;

    bool isInitialized() const override;

    void setProjection( const ViewportParams *viewport ) override;

    void paintContent( QPainter *painter ) override;

    QDialog *configDialog() override;

protected:
    bool eventFilter( QObject *object, QEvent *e ) override;
    void contextMenuEvent( QWidget *w, QContextMenuEvent *e ) override;

 private Q_SLOTS:
    void handleDataUpdate(const GeoDataLineString &points, const QVector<QPointF> &eleData);
    void updateVisiblePoints();
    void forceRepaint();
    void readSettings();
    void writeSettings();
    void toggleZoomToViewport();

    void switchToRouteDataSource();
    void switchToTrackDataSource(int index);
    void switchDataSource(ElevationProfileDataSource *source);


Q_SIGNALS:
    void dataUpdated();

 private:
    ElevationProfileDataSource* m_activeDataSource;
    ElevationProfileRouteDataSource m_routeDataSource;
    ElevationProfileTrackDataSource m_trackDataSource;
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

    friend class ElevationProfileContextMenu;
    ElevationProfileContextMenu*   m_contextMenu;

    MarbleWidget*     m_marbleWidget;

    int               m_firstVisiblePoint;
    int               m_lastVisiblePoint;
    bool              m_zoomToViewport;
    QVector<QPointF>    m_eleData;
    GeoDataLineString m_points;
    qreal             m_minElevation;
    qreal             m_maxElevation;
    qreal             m_gain;
    qreal             m_loss;

    QVector<QPointF> calculateElevationData(const GeoDataLineString &lineString) const;
    void calculateStatistics(const QVector<QPointF> &eleData);
};


}

#endif // ELEVATIONPROFILEFLOATITEM_H
