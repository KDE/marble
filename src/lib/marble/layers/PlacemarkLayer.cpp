//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PlacemarkLayer.h"

#include <QPoint>
#include <QPainter>

#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "GeoDataStyle.h"
#include "GeoPainter.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLatLonAltBox.h"
#include "ViewportParams.h"
#include "VisiblePlacemark.h"
#include "RenderState.h"

using namespace Marble;

bool PlacemarkLayer::m_useXWorkaround = false;

PlacemarkLayer::PlacemarkLayer(QAbstractItemModel *placemarkModel,
                                QItemSelectionModel *selectionModel,
                                MarbleClock *clock, const StyleBuilder *styleBuilder,
                                QObject *parent ) :
    QObject( parent ),
    m_layout( placemarkModel, selectionModel, clock, styleBuilder ),
    m_debugModeEnabled(false)
{
    m_useXWorkaround = testXBug();
    mDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );

    connect( &m_layout, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()) );
}

PlacemarkLayer::~PlacemarkLayer()
{
}

QStringList PlacemarkLayer::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}

qreal PlacemarkLayer::zValue() const
{
    return 2.0;
}

bool PlacemarkLayer::render( GeoPainter *geoPainter, ViewportParams *viewport,
                               const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    QVector<VisiblePlacemark*> visiblePlacemarks = m_layout.generateLayout( viewport );
    // draw placemarks less important first
    QVector<VisiblePlacemark*>::const_iterator visit = visiblePlacemarks.constEnd();
    QVector<VisiblePlacemark*>::const_iterator itEnd = visiblePlacemarks.constBegin();

    QPainter *const painter = geoPainter;
    bool const repeatableX = viewport->currentProjection()->repeatableX();
    int const radius4 = 4 * viewport->radius();
    while ( visit != itEnd ) {
        --visit;

        VisiblePlacemark *const mark = *visit;

        // Intentionally converting positions from floating point to pixel aligned screen grid below
        QRect labelRect( mark->labelRect().toRect() );
        QPoint symbolPos(mark->symbolPosition().toPoint());

        // when the map is such zoomed out that a given place
        // appears many times, we draw one placemark at each
        if (repeatableX) {
            const int symbolX = mark->symbolPosition().x();
            const int textX =   mark->labelRect().x();

            for (int i = symbolX % radius4, width = viewport->width(); i <= width; i += radius4) {
                labelRect.moveLeft(i - symbolX + textX);
                symbolPos.setX(i);

                painter->drawPixmap( symbolPos, mark->symbolPixmap() );
                painter->drawPixmap( labelRect, mark->labelPixmap() );
            }
        } else {
            // simple case, one draw per placemark
            painter->drawPixmap(symbolPos, mark->symbolPixmap());
            painter->drawPixmap(labelRect, mark->labelPixmap());
        }
    }

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

QVector<const GeoDataFeature *> PlacemarkLayer::whichPlacemarkAt( const QPoint &pos )
{
    return m_layout.whichPlacemarkAt( pos );
}

bool PlacemarkLayer::isDebugModeEnabled() const
{
    return m_debugModeEnabled;
}

void PlacemarkLayer::setDebugModeEnabled(bool enabled)
{
    m_debugModeEnabled = enabled;
}

void PlacemarkLayer::setShowPlaces( bool show )
{
    m_layout.setShowPlaces( show );
}

void PlacemarkLayer::setShowCities( bool show )
{
    m_layout.setShowCities( show );
}

void PlacemarkLayer::setShowTerrain( bool show )
{
    m_layout.setShowTerrain( show );
}

void PlacemarkLayer::setShowOtherPlaces( bool show )
{
    m_layout.setShowOtherPlaces( show );
}

void PlacemarkLayer::setShowLandingSites( bool show )
{
    m_layout.setShowLandingSites( show );
}

void PlacemarkLayer::setShowCraters( bool show )
{
    m_layout.setShowCraters( show );
}

void PlacemarkLayer::setShowMaria( bool show )
{
    m_layout.setShowMaria( show );
}

void PlacemarkLayer::requestStyleReset()
{
    m_layout.requestStyleReset();
}


// Test if there a bug in the X server which makes 
// text fully transparent if it gets written on 
// QPixmaps that were initialized by filling them 
// with Qt::transparent

bool PlacemarkLayer::testXBug()
{
    QString  testchar( "K" );
    QFont    font( "Sans Serif", 10 );

    int fontheight = QFontMetrics( font ).height();
    int fontwidth  = QFontMetrics( font ).width(testchar);
    int fontascent = QFontMetrics( font ).ascent();

    QPixmap  pixmap( fontwidth, fontheight );
    pixmap.fill( Qt::transparent );

    QPainter textpainter;
    textpainter.begin( &pixmap );
    textpainter.setPen( QColor( 0, 0, 0, 255 ) );
    textpainter.setFont( font );
    textpainter.drawText( 0, fontascent, testchar );
    textpainter.end();

    QImage image = pixmap.toImage();

    for ( int x = 0; x < fontwidth; ++x ) {
        for ( int y = 0; y < fontheight; ++y ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }
    }

    return true;
}

void PlacemarkLayer::renderDebug(GeoPainter *painter, ViewportParams *viewport, const QVector<VisiblePlacemark *> &placemarks)
{
    painter->save();
    painter->setBrush(QBrush(Qt::NoBrush));
    auto const latLonAltBox = viewport->viewLatLonAltBox();

    typedef QSet<VisiblePlacemark*> Placemarks;
    Placemarks const hidden = Placemarks::fromList(m_layout.visiblePlacemarks()).subtract(Placemarks::fromList(placemarks.toList()));

    for (auto placemark: hidden) {
        bool const inside = latLonAltBox.contains(placemark->coordinates());
        painter->setPen(QPen(QColor(inside ? Qt::red : Qt::darkYellow)));
        painter->drawRect(placemark->boundingBox());
    }

    painter->setPen(QPen(QColor(Qt::blue)));
    for (auto placemark: placemarks) {
        painter->drawRect(placemark->boundingBox());
    }

    painter->setPen(QPen(QColor(Qt::green)));
    for (auto placemark: placemarks) {
        painter->drawRect(placemark->labelRect());
        painter->drawRect(placemark->symbolRect());
    }

    auto const height = painter->fontMetrics().height();
    painter->setPen(QPen(QColor(Qt::black)));
    for (auto placemark: placemarks) {
        QPoint position = placemark->symbolRect().bottomLeft().toPoint() + QPoint(0, qRound(0.8 * height));
        auto const popularity = placemark->placemark()->popularity();
        painter->drawText(position, QStringLiteral("p: %1").arg(popularity));
        position -= QPoint(0, placemark->symbolRect().height() + height);
        auto const zoomLevel = placemark->placemark()->zoomLevel();
        painter->drawText(position, QStringLiteral("z: %1").arg(zoomLevel));
    }

    painter->restore();
}

#include "moc_PlacemarkLayer.cpp"

