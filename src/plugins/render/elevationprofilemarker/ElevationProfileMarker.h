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
    ~ElevationProfileMarker();

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    qreal zValue() const; // Overriding LayerInterface to paint on top of the route

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const;

    void initialize();

    bool isInitialized() const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos, GeoSceneLayer *layer = 0 );

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
