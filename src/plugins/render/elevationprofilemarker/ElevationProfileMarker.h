//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef ELEVATIONPROFILEMARKER_H
#define ELEVATIONPROFILEMARKER_H

#include "RenderPlugin.h"

#include "BillboardGraphicsItem.h"
#include "GeoDataCoordinates.h"
#include "LabelGraphicsItem.h"


namespace Marble
{

class GeoDataObject;
class GeoDataPlacemark;

class ElevationProfileMarker : public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.ElevationProfileMarker")

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( ElevationProfileMarker )

 public:
    explicit ElevationProfileMarker( const MarbleModel *marbleModel = 0 );
    ~ElevationProfileMarker() override;

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

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

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos, GeoSceneLayer *layer = 0 ) override;

 private Q_SLOTS:
    void onGeoObjectAdded( GeoDataObject *object );
    void onGeoObjectRemoved( GeoDataObject *object );

 private:
    GeoDataPlacemark *m_markerPlacemark;
    GeoDataCoordinates m_currentPosition;

    BillboardGraphicsItem m_markerItem;
    LabelGraphicsItem m_markerIcon;
    LabelGraphicsItem m_markerText;
};


}

#endif // ELEVATIONPROFILEMARKER_H
