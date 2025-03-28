// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
//

#include "ElevationProfileMarker.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "GeoPainter.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

#include <QIcon>
#include <QRect>

namespace Marble
{

ElevationProfileMarker::ElevationProfileMarker(const MarbleModel *marbleModel)
    : RenderPlugin(marbleModel)
    , m_markerPlacemark(nullptr)
    , m_markerItem()
    , m_markerIcon(&m_markerItem)
    , m_markerText(&m_markerItem)
{
    if (!marbleModel) {
        return;
    }
    setVisible(false);
    m_markerItem.setCacheMode(MarbleGraphicsItem::ItemCoordinateCache);

    connect(const_cast<MarbleModel *>(marbleModel)->treeModel(), &GeoDataTreeModel::added, this, &ElevationProfileMarker::onGeoObjectAdded);
    connect(const_cast<MarbleModel *>(marbleModel)->treeModel(), &GeoDataTreeModel::removed, this, &ElevationProfileMarker::onGeoObjectRemoved);
}

ElevationProfileMarker::~ElevationProfileMarker() = default;

QStringList ElevationProfileMarker::backendTypes() const
{
    return QStringList(QStringLiteral("elevationprofilemarker"));
}

QString ElevationProfileMarker::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList ElevationProfileMarker::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}

qreal ElevationProfileMarker::zValue() const
{
    return 3.0;
}

QString ElevationProfileMarker::name() const
{
    return tr("Elevation Profile Marker");
}

QString ElevationProfileMarker::guiString() const
{
    return tr("&Elevation Profile Marker");
}

QString ElevationProfileMarker::nameId() const
{
    return QStringLiteral("elevationprofilemarker");
}

QString ElevationProfileMarker::version() const
{
    return QStringLiteral("1.0");
}

QString ElevationProfileMarker::description() const
{
    return tr("Marks the current elevation of the elevation profile on the map.");
}

QString ElevationProfileMarker::copyrightYears() const
{
    return QStringLiteral("2011, 2012");
}

QList<PluginAuthor> ElevationProfileMarker::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"))
                                 << PluginAuthor(QStringLiteral("Florian Eßer"), QStringLiteral("f.esser@rwth-aachen.de"));
}

QIcon ElevationProfileMarker::icon() const
{
    return QIcon(QStringLiteral(":/icons/elevationprofile.png"));
}

void ElevationProfileMarker::initialize()
{
    m_markerIcon.setImage(QImage(QStringLiteral(":/flag-red-mirrored.png")));

    auto topLayout = new MarbleGraphicsGridLayout(1, 2);
    m_markerItem.setLayout(topLayout);
    topLayout->addItem(&m_markerIcon, 0, 0);

    m_markerText.setFrame(LabelGraphicsItem::RoundedRectFrame);
    m_markerText.setPadding(1);
    topLayout->addItem(&m_markerText, 0, 1);
}

bool ElevationProfileMarker::isInitialized() const
{
    return !m_markerIcon.image().isNull();
}

bool ElevationProfileMarker::render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer)
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    if (!m_markerPlacemark)
        return true;

    if (m_currentPosition != m_markerPlacemark->coordinate()) {
        m_currentPosition = m_markerPlacemark->coordinate();

        if (m_currentPosition.isValid()) {
            QString unitString = tr("m");
            int displayScale = 1.0;
            const MarbleLocale::MeasurementSystem measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();
            switch (measurementSystem) {
            case MarbleLocale::MetricSystem:
                /* nothing to do */
                break;
            case MarbleLocale::ImperialSystem:
                unitString = tr("ft");
                displayScale = M2FT;
                break;
            case MarbleLocale::NauticalSystem:
                unitString = tr("ft");
                displayScale = 0.001 * KM2NM * NM2FT;
            }

            QString intervalStr;
            intervalStr.setNum(m_currentPosition.altitude() * displayScale, 'f', 1);
            intervalStr += QLatin1Char(' ') + unitString;
            m_markerText.setText(intervalStr);
        }
    }

    if (m_currentPosition.isValid()) {
        qreal x;
        qreal y;
        qreal lon;
        qreal lat;
        // move the icon by some pixels, so that the pole of the flag sits at the exact point
        int dx = +3 + m_markerItem.size().width() / 2 - m_markerIcon.contentRect().right(); //-4;
        int dy = -6;
        viewport->screenCoordinates(m_currentPosition.longitude(GeoDataCoordinates::Radian), m_currentPosition.latitude(GeoDataCoordinates::Radian), x, y);
        viewport->geoCoordinates(x + dx, y + dy, lon, lat, GeoDataCoordinates::Radian);
        m_markerItem.setCoordinate(GeoDataCoordinates(lon, lat, m_currentPosition.altitude(), GeoDataCoordinates::Radian));

        painter->save();

        m_markerItem.paintEvent(painter, viewport);

        painter->restore();
    }

    return true;
}

void ElevationProfileMarker::onGeoObjectAdded(GeoDataObject *object)
{
    if (m_markerPlacemark)
        return;

    auto document = dynamic_cast<GeoDataDocument *>(object);

    if (!document)
        return;

    if (document->name() != QLatin1StringView("Elevation Profile"))
        return;

    if (document->isEmpty())
        return;

    m_markerPlacemark = dynamic_cast<GeoDataPlacemark *>(document->child(0));

    setVisible(m_markerPlacemark != nullptr);
}

void ElevationProfileMarker::onGeoObjectRemoved(GeoDataObject *object)
{
    auto const document = dynamic_cast<GeoDataDocument *>(object);
    if (!document)
        return;

    if (document->name() != QLatin1StringView("Elevation Profile"))
        return;

    m_markerPlacemark = nullptr;

    Q_EMIT repaintNeeded();
}

}

#include "moc_ElevationProfileMarker.cpp"
