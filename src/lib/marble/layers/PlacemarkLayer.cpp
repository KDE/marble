// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007-2008 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PlacemarkLayer.h"

#include <QPoint>

#include "AbstractProjection.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataStyle.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "RenderState.h"
#include "ViewportParams.h"
#include "VisiblePlacemark.h"
#include "osm/OsmPlacemarkData.h"

#define BATCH_RENDERING

using namespace Marble;

PlacemarkLayer::PlacemarkLayer(QAbstractItemModel *placemarkModel,
                               QItemSelectionModel *selectionModel,
                               MarbleClock *clock,
                               const StyleBuilder *styleBuilder,
                               QObject *parent)
    : QObject(parent)
    , m_layout(placemarkModel, selectionModel, clock, styleBuilder)
    , m_debugModeEnabled(false)
    , m_levelTagDebugModeEnabled(false)
    , m_tileLevel(0)
    , m_debugLevelTag(0)
{
    connect(&m_layout, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()));
}

PlacemarkLayer::~PlacemarkLayer() = default;

QStringList PlacemarkLayer::renderPosition() const
{
    return QStringList(QStringLiteral("PLACEMARKS"));
}

qreal PlacemarkLayer::zValue() const
{
    return 2.0;
}

bool PlacemarkLayer::render(GeoPainter *geoPainter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer)
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    QList<VisiblePlacemark *> visiblePlacemarks = m_layout.generateLayout(viewport, m_tileLevel);
    // draw placemarks less important first
    QList<VisiblePlacemark *>::const_iterator visit = visiblePlacemarks.constEnd();
    QList<VisiblePlacemark *>::const_iterator itEnd = visiblePlacemarks.constBegin();

    QPainter *const painter = geoPainter;

    bool const repeatableX = viewport->currentProjection()->repeatableX();
    int const radius4 = 4 * viewport->radius();

#ifdef BATCH_RENDERING
    QHash<QString, Fragment> hash;
#endif

    while (visit != itEnd) {
        --visit;

        VisiblePlacemark *const mark = *visit;
        if (m_levelTagDebugModeEnabled) {
            if (mark->placemark()->hasOsmData()) {
                QHash<QString, QString>::const_iterator tagIter = mark->placemark()->osmData().findTag(QStringLiteral("level"));
                if (tagIter != mark->placemark()->osmData().tagsEnd()) {
                    const int val = tagIter.value().toInt();
                    if (val != m_debugLevelTag) {
                        continue;
                    }
                }
            }
        }

        // Intentionally converting positions from floating point to pixel aligned screen grid below
        QRect labelRect(mark->labelRect().toRect());
        QPoint symbolPos(mark->symbolPosition().toPoint());

        // when the map is such zoomed out that a given place
        // appears many times, we draw one placemark at each
        if (repeatableX) {
            const int symbolX = symbolPos.x();
            const int textX = labelRect.x();

            for (int i = symbolX % radius4, width = viewport->width(); i <= width; i += radius4) {
                labelRect.moveLeft(i - symbolX + textX);
                symbolPos.setX(i);

                if (!mark->symbolPixmap().isNull()) {
#ifdef BATCH_RENDERING
                    QRect symbolRect = mark->symbolPixmap().rect();
                    QPainter::PixmapFragment pixmapFragment = QPainter::PixmapFragment::create(QPointF(symbolPos + symbolRect.center()), QRectF(symbolRect));

                    auto iter = hash.find(mark->symbolId());
                    if (iter == hash.end()) {
                        Fragment fragment;
                        fragment.pixmap = mark->symbolPixmap();
                        fragment.fragments << pixmapFragment;
                        hash.insert(mark->symbolId(), fragment);
                    } else {
                        auto &fragment = iter.value();
                        fragment.fragments << pixmapFragment;
                    }
#else
                    painter->drawPixmap(symbolPos, mark->symbolPixmap());
#endif
                }
                if (!mark->labelPixmap().isNull()) {
                    painter->drawPixmap(labelRect, mark->labelPixmap());
                }
            }
        } else { // simple case, one draw per placemark

            if (!mark->symbolPixmap().isNull()) {
#ifdef BATCH_RENDERING
                QRect symbolRect = mark->symbolPixmap().rect();
                QPainter::PixmapFragment pixmapFragment = QPainter::PixmapFragment::create(QPointF(symbolPos + symbolRect.center()), QRectF(symbolRect));

                auto iter = hash.find(mark->symbolId());
                if (iter == hash.end()) {
                    Fragment fragment;
                    fragment.pixmap = mark->symbolPixmap();
                    fragment.fragments << pixmapFragment;
                    hash.insert(mark->symbolId(), fragment);
                } else {
                    auto &fragment = iter.value();
                    fragment.fragments << pixmapFragment;
                }
#else
                painter->drawPixmap(symbolPos, mark->symbolPixmap());
#endif
            }
            if (!mark->labelPixmap().isNull()) {
                painter->drawPixmap(labelRect, mark->labelPixmap());
            }
        }
    }

#ifdef BATCH_RENDERING
    for (auto iter = hash.begin(), end = hash.end(); iter != end; ++iter) {
        auto const &fragment = iter.value();
        if (m_debugModeEnabled) {
            QPixmap debugPixmap(fragment.pixmap.size());
            QColor backgroundColor;
            QString idStr = iter.key().section(QLatin1Char('/'), -1);
            if (idStr.length() > 2) {
                idStr.remove(QStringLiteral("shop_"));
                backgroundColor =
                    QColor((10 * (int)(idStr[0].toLatin1())) % 255, (10 * (int)(idStr[1].toLatin1())) % 255, (10 * (int)(idStr[2].toLatin1())) % 255);
            } else {
                backgroundColor = QColor((quint64)(&iter.key()));
            }
            debugPixmap.fill(backgroundColor);
            QPainter pixpainter;
            pixpainter.begin(&debugPixmap);
            pixpainter.drawPixmap(0, 0, fragment.pixmap);
            pixpainter.end();
            iter.value().pixmap = debugPixmap;
        }
        painter->drawPixmapFragments(fragment.fragments.data(), fragment.fragments.size(), fragment.pixmap);
    }
#endif

    if (m_debugModeEnabled) {
        renderDebug(geoPainter, viewport, visiblePlacemarks);
    }

    return true;
}

RenderState PlacemarkLayer::renderState() const
{
    return RenderState(QStringLiteral("Placemarks"));
}

QString PlacemarkLayer::runtimeTrace() const
{
    return m_layout.runtimeTrace();
}

QList<const GeoDataFeature *> PlacemarkLayer::whichPlacemarkAt(const QPoint &pos)
{
    return m_layout.whichPlacemarkAt(pos);
}

bool PlacemarkLayer::hasPlacemarkAt(const QPoint &pos)
{
    return m_layout.hasPlacemarkAt(pos);
}

bool PlacemarkLayer::isDebugModeEnabled() const
{
    return m_debugModeEnabled;
}

void PlacemarkLayer::setDebugModeEnabled(bool enabled)
{
    m_debugModeEnabled = enabled;
}

void PlacemarkLayer::setShowPlaces(bool show)
{
    m_layout.setShowPlaces(show);
}

void PlacemarkLayer::setShowCities(bool show)
{
    m_layout.setShowCities(show);
}

void PlacemarkLayer::setShowTerrain(bool show)
{
    m_layout.setShowTerrain(show);
}

void PlacemarkLayer::setShowOtherPlaces(bool show)
{
    m_layout.setShowOtherPlaces(show);
}

void PlacemarkLayer::setShowLandingSites(bool show)
{
    m_layout.setShowLandingSites(show);
}

void PlacemarkLayer::setShowCraters(bool show)
{
    m_layout.setShowCraters(show);
}

void PlacemarkLayer::setShowMaria(bool show)
{
    m_layout.setShowMaria(show);
}

void PlacemarkLayer::requestStyleReset()
{
    m_layout.requestStyleReset();
}

void PlacemarkLayer::setTileLevel(int tileLevel)
{
    m_tileLevel = tileLevel;
}

void PlacemarkLayer::renderDebug(GeoPainter *painter, ViewportParams *viewport, const QList<VisiblePlacemark *> &placemarks) const
{
    painter->save();
    painter->setFont(QFont(QStringLiteral("Sans Serif"), 7));
    painter->setBrush(QBrush(Qt::NoBrush));
    auto const latLonAltBox = viewport->viewLatLonAltBox();

    using Placemarks = QSet<VisiblePlacemark *>;
    const auto visiblePlacemarks = m_layout.visiblePlacemarks();
    Placemarks const hidden =
        Placemarks(visiblePlacemarks.constBegin(), visiblePlacemarks.constEnd()).subtract(Placemarks(placemarks.constBegin(), placemarks.constEnd()));

    for (auto placemark : hidden) {
        bool const inside = latLonAltBox.contains(placemark->coordinates());
        painter->setPen(QPen(QColor(inside ? Qt::red : Qt::darkYellow)));
        painter->drawRect(placemark->boundingBox());
    }

    painter->setPen(QPen(QColor(Qt::blue)));
    for (auto placemark : placemarks) {
        painter->drawRect(placemark->boundingBox());
    }

    painter->setPen(QPen(QColor(Qt::green)));
    for (auto placemark : placemarks) {
        painter->drawRect(placemark->labelRect());
        painter->drawRect(placemark->symbolRect());
    }

    auto const height = painter->fontMetrics().height();
    painter->setPen(QPen(QColor(Qt::black)));
    for (auto placemark : placemarks) {
        QPoint position = placemark->symbolRect().bottomLeft().toPoint() + QPoint(0, qRound(0.8 * height));
        auto const popularity = placemark->placemark()->popularity();
        painter->drawText(position, QStringLiteral("p: %1").arg(popularity));
        position -= QPoint(0, placemark->symbolRect().height() + height);
        auto const zoomLevel = placemark->placemark()->zoomLevel();
        painter->drawText(position, QStringLiteral("z: %1").arg(zoomLevel));
    }

    painter->restore();
}

void PlacemarkLayer::setLevelTagDebugModeEnabled(bool enabled)
{
    if (m_levelTagDebugModeEnabled != enabled) {
        m_levelTagDebugModeEnabled = enabled;
        Q_EMIT repaintNeeded();
    }
}

bool PlacemarkLayer::levelTagDebugModeEnabled() const
{
    return m_levelTagDebugModeEnabled;
}

void PlacemarkLayer::setDebugLevelTag(int level)
{
    if (m_debugLevelTag != level) {
        m_debugLevelTag = level;
        Q_EMIT repaintNeeded();
    }
}

#include "moc_PlacemarkLayer.cpp"
