//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "squad-interpolation.h"

#include <marble/MarbleWidget.h>
#include <marble/MarbleGlobal.h>
#include <marble/GeoDataLatLonAltBox.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/MarblePlacemarkModel.h>
#include <marble/GeoDataLinearRing.h>
#include <marble/GeoDataTypes.h>
#include <marble/MarbleMath.h>
#include <marble/ViewportParams.h>

#include <QApplication>
#include <QTimer>

namespace Marble
{

MyPaintLayer::MyPaintLayer ( MarbleWidget *widget ) :
    m_widget ( widget ),
    m_fraction ( 0.0 ),
    m_delta( 0.02 ),
    m_index ( 0 )
{
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    m_cities << GeoDataCoordinates(  7.64573, 45.04981, 0.0, degree ); // Torino
    m_cities << GeoDataCoordinates(  8.33439, 49.01673, 0.0, degree ); // Karlsruhe
    m_cities << GeoDataCoordinates( 14.41637, 50.09329, 0.0, degree ); // Praha
    m_cities << GeoDataCoordinates( 15.97254, 45.80268, 0.0, degree ); // Zagred
    addInterpolatedPoint();
}

QStringList MyPaintLayer::renderPosition() const
{
    return QStringList(QStringLiteral("USER_TOOLS"));
}

bool MyPaintLayer::render ( GeoPainter *painter, ViewportParams *viewport, const QString &, GeoSceneLayer * )
{
    if ( m_index < 20 ) {
        // Gray dotted line connects all current cities
        QPen grayPen = Marble::Oxygen::aluminumGray4;
        grayPen.setWidth ( 3 );
        grayPen.setStyle ( Qt::DotLine );
        painter->setPen ( grayPen );
        painter->drawPolyline ( m_cities );
    }

    // Blue circle around each city
    painter->setBrush ( QBrush ( QColor ( Marble::Oxygen::skyBlue4 ) ) );
    painter->setPen ( QColor ( Marble::Oxygen::aluminumGray4 ) );
    for ( int i = 0; i < m_cities.size(); ++i ) {
        painter->drawEllipse ( m_cities[i], 32, 32 );
    }

    if (m_index < 10) {
        // Show how squad interpolation works internally
        Q_ASSERT( m_cities.size() == 4 );
        painter->setBrush ( QBrush ( QColor ( Marble::Oxygen::grapeViolet4 ) ) );
        painter->setPen ( QColor ( Marble::Oxygen::aluminumGray4 ) );
        GeoDataCoordinates a2 = basePoint( m_cities[0], m_cities[1], m_cities[2] );
        painter->drawEllipse ( a2, 8, 8 );
        qreal x, y;
        if ( viewport->screenCoordinates ( a2, x, y ) ) {
            painter->drawText(x+5, y, QStringLiteral("A"));
        }
        GeoDataCoordinates b1 = basePoint( m_cities[1], m_cities[2], m_cities[3] );
        painter->drawEllipse ( b1, 8, 8 );
        if ( viewport->screenCoordinates ( b1, x, y ) ) {
            painter->drawText(x+5, y, QStringLiteral("B"));
        }

        QPen grapePen = Marble::Oxygen::grapeViolet4;
        grapePen.setWidth ( 2 );
        painter->setPen ( grapePen );
        GeoDataLineString string;
        string << m_cities[0] << a2 << b1 << m_cities[3];
        painter->drawPolyline ( string );

        GeoDataCoordinates i1 = m_cities[1].interpolate( m_cities[2], m_fraction-m_delta );
        GeoDataCoordinates i2 = a2.interpolate( b1, m_fraction-m_delta );
        QPen raspberryPen = Marble::Oxygen::burgundyPurple4;
        raspberryPen.setWidth ( 2 );
        painter->setPen ( raspberryPen );
        GeoDataLineString inter;
        inter << i1 << i2;
        painter->drawPolyline ( inter );
    }

    // Green linestring shows interpolation path
    QPen greenPen = Marble::Oxygen::forestGreen4;
    greenPen.setWidth ( 3 );
    painter->setPen ( greenPen );
    painter->drawPolyline ( m_interpolated, QStringLiteral("Squad\nInterpolation"), LineEnd );

    // Increasing city indices with some transparency effect for readability
    QFont font = painter->font();
    font.setBold( true );
    painter->setFont( font );
    QColor blue = QColor ( Marble::Oxygen::skyBlue4 );
    blue.setAlpha( 150 );
    painter->setBrush ( QBrush ( blue ) );
    int const h = painter->fontMetrics().height();
    for ( int i = 0; i < m_cities.size(); ++i ) {
        qreal x, y;
        QString const text = QString::number ( m_index + i );
        int const w = painter->fontMetrics().width( text );
        painter->setPen ( Qt::NoPen );
        painter->drawEllipse ( m_cities[i], 1.5*w, 1.5*h );
        painter->setPen ( QColor ( Marble::Oxygen::aluminumGray4 ) );
        if ( viewport->screenCoordinates ( m_cities[i], x, y ) ) {
            painter->drawText ( x-w/2, y+h/3, text );
        }
    }

    return true;
}

GeoDataLatLonBox MyPaintLayer::center() const
{
    GeoDataLinearRing ring;
    foreach( const GeoDataCoordinates &city, m_cities ) {
        ring << city;
    }
    return ring.latLonAltBox();
}

void MyPaintLayer::addRandomCity ( double minDistance, double maxDistance )
{
    minDistance *= KM2METER;
    maxDistance *= KM2METER;
    GeoDataTreeModel* tree = m_widget->model()->treeModel();
    if ( !tree || tree->rowCount() < 6 || m_cities.isEmpty() ) {
        return;
    }

    // Traverse Marble's internal city database and add a random one
    // which is in the requested distance range to the last one
    for ( int i = 0; i < tree->rowCount(); ++i ) {
        QVariant const data = tree->data ( tree->index ( i, 0 ), MarblePlacemarkModel::ObjectPointerRole );
        GeoDataObject *object = qvariant_cast<GeoDataObject*> ( data );
        Q_ASSERT ( object );
        if ( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument* document = static_cast<GeoDataDocument*> ( object );
            if (document->name() == QLatin1String("cityplacemarks")) {
                QVector<GeoDataPlacemark*> placemarks = document->placemarkList();
                for ( int i = qrand() % placemarks.size(); i < placemarks.size(); ++i ) {
                    double const distance = EARTH_RADIUS * distanceSphere ( m_cities.last(), placemarks[i]->coordinate() );
                    if ( distance >= minDistance && distance <= maxDistance ) {
                        m_cities << placemarks[i]->coordinate();
                        return;
                    }
                }
            }
        }
    }

    addRandomCity();
}

GeoDataCoordinates MyPaintLayer::basePoint( const GeoDataCoordinates &c1, const GeoDataCoordinates &c2, const GeoDataCoordinates &c3 )
{
    Quaternion const a = (c2.quaternion().inverse() * c3.quaternion()).log();
    Quaternion const b = (c2.quaternion().inverse() * c1.quaternion()).log();
    Quaternion const c = c2.quaternion() * ((a+b)*-0.25).exp();
    qreal lon, lat;
    c.getSpherical( lon, lat );
    return GeoDataCoordinates( lon, lat );
}

void MyPaintLayer::addInterpolatedPoint()
{
    while ( m_interpolated.size() > 2.0/m_delta ) {
        m_interpolated.remove ( 0 );
    }

    m_delta = m_index < 20 ? 0.01 : 0.04;
    Q_ASSERT ( m_cities.size() == 4 );
    // Interpolate for the current city
    m_interpolated << m_cities[1].interpolate ( m_cities[0], m_cities[2], m_cities[3], m_fraction );
    m_fraction += m_delta;

    // If current city is done, move one forward
    if ( m_fraction > 1.0 ) {
        m_fraction = 0.0;
        m_cities.remove ( 0 );
        addRandomCity();
        ++m_index;
    }

    // Repaint map, recenter if out of view
    bool hidden;
    qreal x; qreal y;
    if ( m_widget->viewport()->screenCoordinates ( m_interpolated.last(), x, y, hidden ) ) {
        m_widget->update();
    } else {
        m_widget->centerOn ( center() );
    }

    int const timeout = qBound( 0, 150 - 50 * m_index, 150 );
    QTimer::singleShot ( timeout, this, SLOT (addInterpolatedPoint()) );
}

}

int main ( int argc, char** argv )
{
    using namespace Marble;
    QApplication app ( argc, argv );
    MarbleWidget *mapWidget = new MarbleWidget;
    mapWidget->setWindowTitle(QStringLiteral("Marble - Squad Interpolation"));

    // Create and register our paint layer
    MyPaintLayer* layer = new MyPaintLayer ( mapWidget );
    mapWidget->addLayer ( layer );
    mapWidget->centerOn ( layer->center() );

    // Finish widget creation.
    mapWidget->setMapThemeId(QStringLiteral("earth/plain/plain.dgml"));
    mapWidget->setShowCities( false );
    mapWidget->setShowCrosshairs( false );
    mapWidget->setShowOtherPlaces( false );
    mapWidget->setShowPlaces( false );
    mapWidget->setShowTerrain( false );
    mapWidget->show();

    return app.exec();
}

#include "moc_squad-interpolation.cpp"
