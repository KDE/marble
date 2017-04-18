//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoLineStringGraphicsItem.h"

#include "GeoDataLineString.h"
#include "GeoDataLineStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolyStyle.h"
#include "GeoPainter.h"
#include "StyleBuilder.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"
#include "GeoDataColorStyle.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "OsmPlacemarkData.h"

#include <qmath.h>
#include <QPainterPathStroker>

namespace Marble
{

const GeoDataStyle *GeoLineStringGraphicsItem::s_previousStyle = 0;
bool GeoLineStringGraphicsItem::s_paintInline = true;
bool GeoLineStringGraphicsItem::s_paintOutline = true;

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem(const GeoDataPlacemark *placemark,
                                                     const GeoDataLineString *lineString) :
    GeoGraphicsItem(placemark),
    m_lineString(lineString),
    m_renderLineString(lineString),
    m_renderLabel(false),
    m_penWidth(0.0),
    m_name(placemark->name())
{
    QString const category = StyleBuilder::visualCategoryName(placemark->visualCategory());
    QStringList paintLayers;
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/outline");
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/inline");
    if (!m_name.isEmpty()) {
        paintLayers << QLatin1String("LineString/") + category + QLatin1String("/label");
    }
    setPaintLayers(paintLayers);
}

GeoLineStringGraphicsItem::~GeoLineStringGraphicsItem()
{
    qDeleteAll(m_cachedPolygons);
}


void GeoLineStringGraphicsItem::setLineString( const GeoDataLineString* lineString )
{
    m_lineString = lineString;
    m_renderLineString = lineString;
}

const GeoDataLineString *GeoLineStringGraphicsItem::lineString() const
{
    return m_lineString;
}

GeoDataLineString GeoLineStringGraphicsItem::merge(const QVector<const GeoDataLineString *> &lineStrings_)
{
    if (lineStrings_.isEmpty()) {
        return GeoDataLineString();
    }

    Q_ASSERT(!lineStrings_.isEmpty());
    auto lineStrings = lineStrings_;
    GeoDataLineString result = *lineStrings.first();
    lineStrings.pop_front();
    for (bool matched = true; matched && !lineStrings.isEmpty();) {
        matched = false;
        for (auto lineString: lineStrings) {
            if (canMerge(result.first(), lineString->first())) {
                result.remove(0);
                result.reverse();
                result << *lineString;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            } else if (canMerge(result.last(), lineString->first())) {
                result.remove(result.size()-1);
                result << *lineString;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            } else if (canMerge(result.first(), lineString->last())) {
                GeoDataLineString behind = result;
                result = *lineString;
                behind.remove(0);
                result << behind;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            } else if (canMerge(result.last(), lineString->last())) {
                GeoDataLineString behind = *lineString;
                behind.reverse();
                behind.remove(0);
                result << behind;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            }
        }

        if (!matched) {
            return GeoDataLineString();
        }
    }
    return lineStrings.isEmpty() ? result : GeoDataLineString();
}

void GeoLineStringGraphicsItem::setMergedLineString(const GeoDataLineString &mergedLineString)
{
    m_mergedLineString = mergedLineString;
    m_renderLineString = mergedLineString.isEmpty() ? m_lineString : &m_mergedLineString;
}

const GeoDataLatLonAltBox& GeoLineStringGraphicsItem::latLonAltBox() const
{
    return m_renderLineString->latLonAltBox();
}

void GeoLineStringGraphicsItem::paint(GeoPainter* painter, const ViewportParams* viewport , const QString &layer, int tileLevel)
{
    setRenderContext(RenderContext(tileLevel));

    if (layer.endsWith(QLatin1String("/outline"))) {
        qDeleteAll(m_cachedPolygons);
        m_cachedPolygons.clear();
        m_cachedRegion = QRegion();
        painter->polygonsFromLineString(*m_renderLineString, m_cachedPolygons);
        if (m_cachedPolygons.empty()) {
            return;
        }
        if (painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality) {
            paintOutline(painter, viewport);
        }
    } else if (layer.endsWith(QLatin1String("/inline"))) {
        if (m_cachedPolygons.empty()) {
            return;
        }
        paintInline(painter, viewport);
    } else if (layer.endsWith(QLatin1String("/label"))) {
        if (!m_cachedPolygons.empty()) {
            if (m_renderLabel) {
                paintLabel(painter, viewport);
            }
        }
    } else {
        qDeleteAll(m_cachedPolygons);
        m_cachedPolygons.clear();
        m_cachedRegion = QRegion();
        painter->polygonsFromLineString(*m_renderLineString, m_cachedPolygons);
        if (m_cachedPolygons.empty()) {
            return;
        }
        for(const QPolygonF* itPolygon: m_cachedPolygons) {
            painter->drawPolyline(*itPolygon);
        }
    }
}

bool GeoLineStringGraphicsItem::contains(const QPoint &screenPosition, const ViewportParams *) const
{
    if (m_penWidth <= 0.0) {
        return false;
    }

    if (m_cachedRegion.isNull()) {
        QPainterPath painterPath;
        for (auto polygon: m_cachedPolygons) {
            painterPath.addPolygon(*polygon);
        }
        QPainterPathStroker stroker;
        qreal const margin = 6.0;
        stroker.setWidth(m_penWidth + margin);
        QPainterPath strokePath = stroker.createStroke(painterPath);
        m_cachedRegion = QRegion(strokePath.toFillPolygon().toPolygon(), Qt::WindingFill);
    }
    return m_cachedRegion.contains(screenPosition);
}

void GeoLineStringGraphicsItem::handleRelationUpdate(const QVector<const GeoDataRelation *> &relations)
{
    QHash<GeoDataRelation::RelationType, QStringList> names;
    for (auto relation: relations) {
        auto const ref = relation->osmData().tagValue(QStringLiteral("ref"));
        if (relation->isVisible() && !ref.isEmpty()) {
            names[relation->relationType()] << ref;
        }
    }
    if (names.isEmpty()) {
        m_name = feature()->name();
    } else {
        QStringList namesList;
        for (auto iter = names.begin(); iter != names.end(); ++iter) {
            QString value;
            switch (iter.key()) {
            case GeoDataRelation::UnknownType:
            case GeoDataRelation::RouteRoad: break;
            case GeoDataRelation::RouteDetour: value = tr("Detour"); break;
            case GeoDataRelation::RouteFerry: value = tr("Ferry Route"); break;
            case GeoDataRelation::RouteTrain: value = tr("Train"); break;
            case GeoDataRelation::RouteSubway: value = tr("Subway"); break;
            case GeoDataRelation::RouteTram: value = tr("Tram"); break;
            case GeoDataRelation::RouteBus: value = tr("Bus"); break;
            case GeoDataRelation::RouteTrolleyBus: value = tr("Trolley Bus"); break;
            case GeoDataRelation::RouteBicycle: value = tr("Bicycle Route"); break;
            case GeoDataRelation::RouteMountainbike: value = tr("Mountainbike Route"); break;
            case GeoDataRelation::RouteFoot: value = tr("Walking Route"); break;
            case GeoDataRelation::RouteHiking: value = tr("Hiking Route"); break;
            case GeoDataRelation::RouteHorse: value = tr("Bridleway"); break;
            case GeoDataRelation::RouteInlineSkates: value = tr("Inline Skates Route"); break;
            case GeoDataRelation::RouteSkiDownhill: value = tr("Downhill Piste"); break;
            case GeoDataRelation::RouteSkiNordic: value = tr("Nordic Ski Trail"); break;
            case GeoDataRelation::RouteSkitour: value = tr("Skitour"); break;
            case GeoDataRelation::RouteSled: value = tr("Sled Trail"); break;
            }

            QStringList &references = iter.value();
            std::sort(references.begin(), references.end());
            auto const last = std::unique(references.begin(), references.end());
            references.erase(last, references.end());
            auto const routes = references.join(", ");
            namesList << (value.isEmpty() ? routes : QStringLiteral("%1 %2").arg(value, routes));
        }
        auto const allRoutes = namesList.join(QStringLiteral("; "));
        if (feature()->name().isEmpty()) {
            m_name = allRoutes;
        } else {
            m_name = QStringLiteral("%1 (%2)").arg(feature()->name(), allRoutes);
        }
    }
}

void GeoLineStringGraphicsItem::paintInline(GeoPainter* painter, const ViewportParams* viewport)
{
    if ( ( !viewport->resolves( m_renderLineString->latLonAltBox(), 2) ) ) {
        return;
    }

    if (s_previousStyle != style().data()) {
        s_paintInline = configurePainterForLine(painter, viewport, false);
    }
    s_previousStyle = style().data();

    if (s_paintInline) {
      m_renderLabel = painter->pen().widthF() >= 6.0f;
      m_penWidth = painter->pen().widthF();
      for(const QPolygonF* itPolygon: m_cachedPolygons) {
          painter->drawPolyline(*itPolygon);
      }
    }
}

void GeoLineStringGraphicsItem::paintOutline(GeoPainter *painter, const ViewportParams *viewport) const
{
    if ( ( !viewport->resolves( m_renderLineString->latLonAltBox(), 2) ) ) {
        return;
    }

    if (s_previousStyle != style().data()) {
        s_paintOutline = configurePainterForLine(painter, viewport, true);
    }
    s_previousStyle = style().data();

    if (s_paintOutline) {
        for(const QPolygonF* itPolygon: m_cachedPolygons) {
            painter->drawPolyline(*itPolygon);
        }
    }

}

void GeoLineStringGraphicsItem::paintLabel(GeoPainter *painter, const ViewportParams *viewport) const
{
    if ( ( !viewport->resolves( m_renderLineString->latLonAltBox(), 2) ) ) {
        return;
    }

    LabelPositionFlags labelPositionFlags = NoLabel;
    bool isValid  = configurePainterForLabel(painter, viewport, labelPositionFlags);

    if (isValid) {
        GeoDataStyle::ConstPtr style = this->style();

        // Activate the lines below to paint a label background which
        // prevents antialiasing overpainting glitches, but leads to
        // other glitches.
        //QColor const color = style->polyStyle().paintedColor();
        //painter->setBackground(QBrush(color));
        //painter->setBackgroundMode(Qt::OpaqueMode);

        const GeoDataLabelStyle& labelStyle = style->labelStyle();
        painter->drawLabelsForPolygons(m_cachedPolygons, m_name, FollowLine,
                               labelStyle.paintedColor());
    }
}


bool  GeoLineStringGraphicsItem::configurePainterForLine(GeoPainter *painter, const ViewportParams *viewport, const bool isOutline) const
{
    QPen currentPen = painter->pen();
    GeoDataStyle::ConstPtr style = this->style();
    if (!style) {
        painter->setPen( QPen() );
        painter->setBackground(QBrush(Qt::transparent));
        painter->setBackgroundMode(Qt::TransparentMode);
    }
    else {
        if (isOutline && !style->polyStyle().outline()) {
            return false;
        }

        const GeoDataLineStyle& lineStyle = style->lineStyle();

        // To save performance we avoid making changes to the painter's pen.
        // So we first take a copy of the actual painter pen, make changes to it
        // and only if the resulting pen is different from the actual pen
        // we replace the painter's pen with our new pen.

        // We want to avoid the mandatory detach in QPen::setColor(),
        // so we carefully check whether applying the setter is needed
        const QColor linePaintedColor = (!isOutline && (lineStyle.cosmeticOutline() && lineStyle.penStyle() == Qt::SolidLine))
                                        ? style->polyStyle().paintedColor()
                                        : lineStyle.paintedColor();
        if (currentPen.color() != linePaintedColor) {
            if (linePaintedColor.alpha() == 255) {
                currentPen.setColor(linePaintedColor);
            }
            else {
                if ( painter->mapQuality() != Marble::HighQuality
                        && painter->mapQuality() != Marble::PrintQuality ) {
                    QColor penColor = linePaintedColor;
                    if (penColor.alpha() != 0) {
                        penColor.setAlpha( 255 );
                    }
                    if (currentPen.color() != penColor) {
                        currentPen.setColor( penColor );
                    }
                }
                else {
                    currentPen.setColor(linePaintedColor);
                }
            }
        }

        const float lineWidth = lineStyle.width();
        const float linePhysicalWidth = lineStyle.physicalWidth();
        float newLineWidth = lineWidth;
        if (linePhysicalWidth != 0.0) {
          const float scaledLinePhysicalWidth = float(viewport->radius()) / EARTH_RADIUS * linePhysicalWidth;
          newLineWidth = scaledLinePhysicalWidth > lineWidth
                         ? scaledLinePhysicalWidth
                         : lineWidth;
        }

        if (!isOutline && lineStyle.cosmeticOutline() && lineStyle.penStyle() == Qt::SolidLine) {
            if (newLineWidth > 2.5) {
              newLineWidth -= 2.0;
            }
        }

        const qreal lineDrawThreshold = isOutline ? 2.5 : 0.5;

        // We want to avoid the mandatory detach in QPen::setWidthF(),
        // so we carefully check whether applying the setter is needed
        if (currentPen.widthF() != newLineWidth && newLineWidth != 0.0) {
          if (newLineWidth < lineDrawThreshold) {
              if (painter->pen().style() != Qt::NoPen) {
                  painter->setPen(Qt::NoPen);
              }
              return false; // Don't draw any outline and abort painter configuration early
          }
          currentPen.setWidthF(newLineWidth);
        }

        // No need to avoid detaches inside QPen::setCapsStyle() since Qt does that for us
        const Qt::PenCapStyle lineCapStyle = lineStyle.capStyle();
        currentPen.setCapStyle(lineCapStyle);

        // No need to avoid detaches inside QPen::setStyle() since Qt does that for us
        if (painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality) {
            const Qt::PenStyle linePenStyle = lineStyle.penStyle();
            currentPen.setStyle(linePenStyle);

            if (linePenStyle == Qt::CustomDashLine) {
                // We want to avoid the mandatory detach in QPen::setDashPattern(),
                // so we carefully check whether applying the setter is needed
                if (currentPen.dashPattern() != lineStyle.dashPattern()) {
                  currentPen.setDashPattern(lineStyle.dashPattern());
                }
            }
        } else {
            currentPen.setStyle(Qt::SolidLine);
        }

        if ( painter->pen() != currentPen ) {
            painter->setPen( currentPen );
        }

        // Set the background

        if (!isOutline) {
          if (lineStyle.background()) {
              QBrush brush = painter->background();
              brush.setColor(style->polyStyle().paintedColor());
              painter->setBackground( brush );

              painter->setBackgroundMode( Qt::OpaqueMode );
          }
          else {
              painter->setBackground(QBrush(Qt::transparent));
              painter->setBackgroundMode(Qt::TransparentMode);
          }
        }
        else {
            painter->setBackground(QBrush(Qt::transparent));
            painter->setBackgroundMode(Qt::TransparentMode);
        }
    }

    return true;
}

bool GeoLineStringGraphicsItem::configurePainterForLabel(GeoPainter *painter,  const ViewportParams *viewport, LabelPositionFlags &labelPositionFlags) const
{
  QPen currentPen = painter->pen();
  GeoDataStyle::ConstPtr style = this->style();
  if (!style) {
      painter->setPen( QPen() );
  }
  else {
      const GeoDataLineStyle& lineStyle = style->lineStyle();

      // To save performance we avoid making changes to the painter's pen.
      // So we first take a copy of the actual painter pen, make changes to it
      // and only if the resulting pen is different from the actual pen
      // we replace the painter's pen with our new pen.

      // We want to avoid the mandatory detach in QPen::setColor(),
      // so we carefully check whether applying the setter is needed

      const float lineWidth = lineStyle.width();
      const float linePhysicalWidth = lineStyle.physicalWidth();
      float newLineWidth = lineWidth;
      if (linePhysicalWidth != 0.0) {
        const float scaledLinePhysicalWidth = float(viewport->radius()) / EARTH_RADIUS * linePhysicalWidth;
        newLineWidth = scaledLinePhysicalWidth > lineWidth
                       ? scaledLinePhysicalWidth
                       : lineWidth;
      }

      // We want to avoid the mandatory detach in QPen::setWidthF(),
      // so we carefully check whether applying the setter is needed
      if (currentPen.widthF() != newLineWidth && newLineWidth != 0.0) {
        if (newLineWidth < 6.0) {
            return false; // Don't draw any labels and abort painter configuration early
        }
        currentPen.setWidthF(newLineWidth);
      }


      if ( painter->pen() != currentPen ) {
          painter->setPen( currentPen );
      }
//        else qDebug() << "Detach and painter change successfully Avoided!" << Q_FUNC_INFO;

      if (painter->brush().color() != Qt::transparent) {
          painter->setBrush(QColor(Qt::transparent));
      }
      if (painter->backgroundMode() == Qt::OpaqueMode) {
          painter->setBackgroundMode(Qt::TransparentMode);
          painter->setBackground(QBrush(Qt::transparent));
      }

      // label styles
      const GeoDataLabelStyle& labelStyle = style->labelStyle();
      painter->setFont(labelStyle.font() );
      switch (labelStyle.alignment()) {
      case GeoDataLabelStyle::Corner:
      case GeoDataLabelStyle::Right:
          labelPositionFlags |= LineStart;
          break;
      case GeoDataLabelStyle::Center:
          labelPositionFlags |= LineCenter;
          break;
      }
  }

  return true;
}

bool GeoLineStringGraphicsItem::canMerge(const GeoDataCoordinates &a, const GeoDataCoordinates &b)
{
    return distanceSphere(a, b) * EARTH_RADIUS < 0.1;
}

}
