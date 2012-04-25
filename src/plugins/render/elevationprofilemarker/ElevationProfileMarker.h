//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright      2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef ELEVATIONPROFILEMARKER_H
#define ELEVATIONPROFILEMARKER_H

#include "RenderPlugin.h"

#include "GeoDataCoordinates.h"
#include "GeoGraphicsItem.h"
#include "LabelGraphicsItem.h"


namespace Marble
{

class GeoDataObject;
class GeoDataPlacemark;

class ElevationProfileMarker : public RenderPlugin
{
    Q_OBJECT

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( ElevationProfileMarker )

 public:
    ElevationProfileMarker();
    explicit ElevationProfileMarker( const MarbleModel *marbleModel );
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

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon() const;

    void initialize();

    bool isInitialized() const;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos, GeoSceneLayer *layer = 0 );

 private Q_SLOTS:
    void onGeoObjectAdded( GeoDataObject *object );
    void onGeoObjectRemoved( GeoDataObject *object );

 private:
    int m_fontHeight;

    GeoDataPlacemark *m_markerPlacemark;
    GeoDataCoordinates m_currentPosition;

    GeoGraphicsItem m_markerIconContainer;
    GeoGraphicsItem m_markerTextContainer;
    LabelGraphicsItem m_markerIcon;
    LabelGraphicsItem m_markerText;
};


}

#endif // ELEVATIONPROFILEMARKER_H
