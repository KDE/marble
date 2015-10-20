//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoPolygonGraphicsItem.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "GeoDataTypes.h"
#include "GeoDataPlacemark.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"
#include "MarbleDirs.h"
#include "OsmPlacemarkData.h"

#include <QVector2D>
#include <QtCore/qmath.h>

namespace Marble
{

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( const GeoDataFeature *feature, const GeoDataPolygon* polygon )
        : GeoGraphicsItem( feature ),
          m_polygon( polygon ),
          m_ring( 0 ),
          m_buildingHeight(0.0)
{
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( const GeoDataFeature *feature, const GeoDataLinearRing* ring )
        : GeoGraphicsItem( feature ),
          m_polygon( 0 ),
          m_ring( ring ),
          m_buildingHeight(0.0)
{
}

const float GeoPolygonGraphicsItem::s_decorationZValue = -0.001;

void GeoPolygonGraphicsItem::createDecorations()
{
    if (isDecoration() || (!m_polygon && !m_ring )) {
        return;
    }

    GeoPolygonGraphicsItem *fake3D;

    switch ( feature()->visualCategory() ) {
    case GeoDataFeature::Building:
        //case GeoDataFeature::AccomodationCamping:
    case GeoDataFeature::AccomodationHostel:
    case GeoDataFeature::AccomodationHotel:
    case GeoDataFeature::AccomodationMotel:
    case GeoDataFeature::AccomodationYouthHostel:
    case GeoDataFeature::AmenityLibrary:
    case GeoDataFeature::EducationCollege:
    case GeoDataFeature::EducationSchool:
    case GeoDataFeature::EducationUniversity:
    case GeoDataFeature::FoodBar:
    case GeoDataFeature::FoodBiergarten:
    case GeoDataFeature::FoodCafe:
    case GeoDataFeature::FoodFastFood:
    case GeoDataFeature::FoodPub:
    case GeoDataFeature::FoodRestaurant:
    case GeoDataFeature::HealthDoctors:
    case GeoDataFeature::HealthHospital:
    case GeoDataFeature::HealthPharmacy:
    case GeoDataFeature::MoneyAtm:
    case GeoDataFeature::MoneyBank:
    case GeoDataFeature::ShopBeverages:
    case GeoDataFeature::ShopHifi:
    case GeoDataFeature::ShopSupermarket:
        //case GeoDataFeature::TouristAttraction:
        //case GeoDataFeature::TouristCastle:
    case GeoDataFeature::TouristCinema:
        //case GeoDataFeature::TouristMonument:
    case GeoDataFeature::TouristMuseum:
        //case GeoDataFeature::TouristRuin:
    case GeoDataFeature::TouristTheatre:
        //case GeoDataFeature::TouristThemePark:
        //case GeoDataFeature::TouristViewPoint:
        //case GeoDataFeature::TouristZoo:
    case GeoDataFeature::ReligionPlaceOfWorship:
    case GeoDataFeature::ReligionBahai:
    case GeoDataFeature::ReligionBuddhist:
    case GeoDataFeature::ReligionChristian:
    case GeoDataFeature::ReligionHindu:
    case GeoDataFeature::ReligionJain:
    case GeoDataFeature::ReligionJewish:
    case GeoDataFeature::ReligionShinto:
    case GeoDataFeature::ReligionSikh:
    {
        fake3D = m_polygon ? new GeoPolygonGraphicsItem( feature(), m_polygon ) : new GeoPolygonGraphicsItem( feature(), m_ring );
        fake3D->setZValue(this->zValue() + s_decorationZValue);
        double const height = extractBuildingHeight(8.0);
        m_buildingHeight = qBound(1.0, height, 1000.0);
        fake3D->m_buildingHeight = m_buildingHeight;
        Q_ASSERT(m_buildingHeight > 0.0);
    }
        break;

    default:
        fake3D = nullptr;
        break;
    }

    this->addDecoration(fake3D);
}

QPointF GeoPolygonGraphicsItem::buildingOffset(const QPointF &point, const ViewportParams *viewport, bool* isCameraAboveBuilding) const
{
    qreal const cameraFactor = 0.5 * tan(0.5 * 110 * DEG2RAD);
    Q_ASSERT(m_buildingHeight > 0.0);
    qreal const buildingFactor = m_buildingHeight / EARTH_RADIUS;

    qreal const cameraHeightPixel = viewport->width() * cameraFactor;
    qreal buildingHeightPixel = viewport->radius() * buildingFactor;
    if (isCameraAboveBuilding) {
        *isCameraAboveBuilding = cameraHeightPixel > buildingHeightPixel;
    }

    qreal const offsetX = point.x() - viewport->width() / 2.0;
    qreal const xx = cameraHeightPixel / offsetX;
    qreal const xy = (cameraHeightPixel-buildingHeightPixel) / offsetX;
    qreal const shiftX = (cameraHeightPixel-buildingHeightPixel) * (xx-xy)/(sqrt((xx*xx+1)*(xy*xy+1)));
    // The following lines calculate the same result, but are potentially slower due
    // to using more trigonometric method calls
    // qreal const alpha1 = atan2(offsetX, cameraHeightPixel);
    // qreal const alpha2 = atan2(offsetX, cameraHeightPixel-buildingHeightPixel);
    // qreal const shiftX = 2 * (cameraHeightPixel-buildingHeightPixel) * sin(0.5*(alpha2-alpha1));

    qreal const offsetY = point.y() - viewport->height() / 2.0;
    qreal const yx = cameraHeightPixel / offsetY;
    qreal const yy = (cameraHeightPixel-buildingHeightPixel) / offsetY;
    qreal const shiftY = (cameraHeightPixel-buildingHeightPixel) * (yx-yy)/(sqrt((yx*yx+1)*(yy*yy+1)));

    return QPointF(shiftX, shiftY);
}

double GeoPolygonGraphicsItem::extractBuildingHeight(double defaultValue) const
{
    if (feature()->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
        GeoDataPlacemark const * placemark = static_cast<GeoDataPlacemark const *>(feature());
        if (placemark->osmData().containsTagKey("height")) {
            /** @todo Also parse non-SI units, see https://wiki.openstreetmap.org/wiki/Key:height#Height_of_buildings */
            QString const heightValue = placemark->osmData().tagValue("height").replace(" meters", QString()).replace(" m", QString());
            bool extracted = false;
            double height = heightValue.toDouble(&extracted);
            return extracted ? height : defaultValue;
        } else if (placemark->osmData().containsTagKey("building:levels")) {
            int const levels = placemark->osmData().tagValue("building:levels").toInt();
            int const skipLevels = placemark->osmData().tagValue("building:min_level").toInt();
            /** @todo Is 35 as an upper bound for the number of levels sane? */
            return 3.0 * qBound(1, 1+levels-skipLevels, 35);
        }
    }

    return defaultValue;
}

const GeoDataLatLonAltBox& GeoPolygonGraphicsItem::latLonAltBox() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox();
    } else if ( m_ring ) {
        return m_ring->latLonAltBox();
    } else {
        return GeoGraphicsItem::latLonAltBox();
    }
}

void GeoPolygonGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport )
{
    painter->save();

    bool const isBuildingFrame = isDecoration();
    bool const isBuildingRoof = !isDecoration() && !decorations().isEmpty();

    QPen currentPen = painter->pen();

    if ( !style() ) {
        painter->setPen( QPen() );
    }
    else {
        if ( !style()->polyStyle().outline() || isBuildingFrame ) {
            currentPen.setColor( Qt::transparent );
        }
        else {
            if ( currentPen.color() != style()->lineStyle().paintedColor() ||
                 currentPen.widthF() != style()->lineStyle().width() ) {
                currentPen.setColor( style()->lineStyle().paintedColor() );
                currentPen.setWidthF( style()->lineStyle().width() );
            }

            if ( currentPen.capStyle() != style()->lineStyle().capStyle() )
                currentPen.setCapStyle( style()->lineStyle().capStyle() );

            if ( currentPen.style() != style()->lineStyle().penStyle() )
                currentPen.setStyle( style()->lineStyle().penStyle() );
        }

        if ( painter->pen() != currentPen )
            painter->setPen( currentPen );

        if ( !style()->polyStyle().fill() ) {
            if ( painter->brush().color() != Qt::transparent )
                painter->setBrush( QColor( Qt::transparent ) );
        }
        else {
            if ( isBuildingFrame ) {
                painter->setBrush( style()->polyStyle().paintedColor().darker(150) );
            } else if ( painter->brush().color() != style()->polyStyle().paintedColor() ) {
                QImage textureImage = style()->polyStyle().textureImage();
                if( !textureImage.isNull()){
                    GeoDataCoordinates coords = latLonAltBox().center();
                    qreal x, y;
                    viewport->screenCoordinates(coords, x, y);
                    if (m_cachedTexturePath != style()->polyStyle().texturePath() || m_cachedTextureColor != style()->polyStyle().paintedColor() ) {
                        m_cachedTexture = QImage ( textureImage.size(), QImage::Format_ARGB32_Premultiplied );
                        m_cachedTexture.fill(style()->polyStyle().paintedColor());
                        QPainter imagePainter(&m_cachedTexture );
                        imagePainter.drawImage(0, 0, textureImage);
                        imagePainter.end();
                        m_cachedTexturePath = style()->polyStyle().texturePath();
                        m_cachedTextureColor = style()->polyStyle().paintedColor();
                    }
                    QBrush brush;
                    brush.setTextureImage(m_cachedTexture);
                    QTransform transform;
                    brush.setTransform(transform.translate(x,y));
                    painter->setBrush(brush);
                } else {
                    painter->setBrush( style()->polyStyle().paintedColor() );
                }
            }
        }
    }

    bool drawAccurate3D = false;
    bool isCameraAboveBuilding = false;
    if (isBuildingFrame || isBuildingRoof) {
        QPointF offsetAtCorner = buildingOffset(QPointF(0, 0), viewport, &isCameraAboveBuilding);
        qreal maxOffset = qMax( qAbs( offsetAtCorner.x() ), qAbs( offsetAtCorner.y() ) );
        drawAccurate3D = maxOffset > 5.0;
    }

    if ( isBuildingFrame ) {
        QVector<QPolygonF*> polygons;
        if (m_polygon) {
            viewport->screenCoordinates(m_polygon->outerBoundary(), polygons);
        } else if (m_ring) {
            viewport->screenCoordinates(*m_ring, polygons);
        }
        foreach(QPolygonF* polygon, polygons) {
            if (polygon->isEmpty()) {
                continue;
            }
            if ( drawAccurate3D && isCameraAboveBuilding ) {
                int const size = polygon->size();
                QPointF & a = (*polygon)[0];
                QPointF centerBuildingOffset = buildingOffset(polygon->boundingRect().center(), viewport);
                if (painter->mapQuality() == MapQuality::HighQuality || qAbs(centerBuildingOffset.x()) > 5.0 || qAbs(centerBuildingOffset.y()) > 5.0) {
                    QPointF shiftA = a + buildingOffset(a, viewport);;
                    for (int i=1; i<size; ++i) {
                        QPointF const & b = (*polygon)[i];
                        QPointF const shiftB = b + buildingOffset(b, viewport);
                        QPolygonF buildingSide = QPolygonF() << a << shiftA << shiftB << b;
                        painter->drawPolygon(buildingSide);
                        a = b;
                        shiftA = shiftB;
                    }
                }
                else if ( qAbs(centerBuildingOffset.x()) > 1.4 || qAbs(centerBuildingOffset.y()) > 1.4) {
                    QPolygonF offsetPolygon;
                    offsetPolygon = polygon->translated(-0.25*centerBuildingOffset.x(),-0.25*centerBuildingOffset.y());
                    painter->drawPolygon(offsetPolygon);
                }
            } else {
                painter->drawPolygon(*polygon);
            }
        }
        qDeleteAll(polygons);
    } else if (isBuildingRoof) {
        if (!isCameraAboveBuilding) {
            painter->restore();
            return; // do not render roof if we look inside the building
        }

        QVector<QPolygonF*> polygons;
        if (m_polygon) {
            viewport->screenCoordinates(m_polygon->outerBoundary(), polygons);
        } else if (m_ring) {
            viewport->screenCoordinates(*m_ring, polygons);
        }
        bool const hasIcon = !style()->iconStyle().iconPath().isEmpty();
        qreal maxSize(0.0);
        QPointF roofCenter;
        foreach(QPolygonF* polygon, polygons) {
            QRectF const boundingRect = polygon->boundingRect();
            if (hasIcon) {
                QSizeF const polygonSize = boundingRect.size();
                qreal size = polygonSize.width() * polygonSize.height();
                if (size > maxSize) {
                    maxSize = size;
                    roofCenter = boundingRect.center() + buildingOffset(boundingRect.center(), viewport);
                }
            }
            if ( drawAccurate3D) {
                QPolygonF buildingRoof;
                foreach(const QPointF &point, *polygon) {
                    buildingRoof << point + buildingOffset(point, viewport);
                }
                painter->drawPolygon(buildingRoof);
            } else {
                QPointF const offset = buildingOffset(boundingRect.center(), viewport);
                painter->translate(offset);
                painter->drawPolygon(*polygon);
            }
        }
        if (hasIcon && !roofCenter.isNull()) {
            QImage const icon = style()->iconStyle().scaledIcon();
            QPointF const iconCenter(icon.size().width()/2.0, icon.size().height()/2.0);
            painter->drawImage(roofCenter-iconCenter, icon);
        }
        qDeleteAll(polygons);
    } else {
        if ( m_polygon ) {
            painter->drawPolygon( *m_polygon );
        } else if ( m_ring ) {
            painter->drawPolygon( *m_ring );
        }

        bool const hasIcon = !style()->iconStyle().iconPath().isEmpty();
        if (hasIcon) {
            QImage const icon = style()->iconStyle().scaledIcon();
            painter->drawImage(latLonAltBox().center(), icon);
        }
    }

    painter->restore();
}

}
