// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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

    Q_INTERFACES(Marble::RenderPluginInterface)

    MARBLE_PLUGIN(ElevationProfileMarker)

public:
    explicit ElevationProfileMarker(const MarbleModel *marbleModel = nullptr);
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

    QList<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    void initialize() override;

    bool isInitialized() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = nullptr) override;

private Q_SLOTS:
    void onGeoObjectAdded(GeoDataObject *object);
    void onGeoObjectRemoved(GeoDataObject *object);

private:
    GeoDataPlacemark *m_markerPlacemark = nullptr;
    GeoDataCoordinates m_currentPosition;

    BillboardGraphicsItem m_markerItem;
    LabelGraphicsItem m_markerIcon;
    LabelGraphicsItem m_markerText;
};

}

#endif // ELEVATIONPROFILEMARKER_H
