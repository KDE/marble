//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PlacemarkLayout.h"

#include <QAbstractItemModel>
#include <QList>
#include <QPoint>
#include <QVector>
#include <QVectorIterator>
#include <QFont>
#include <QFontMetrics>
#include <QItemSelectionModel>
#include <qmath.h>

#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"

#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "PlacemarkLayer.h"
#include "MarbleClock.h"
#include "MarblePlacemarkModel.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"
#include "TileId.h"
#include "TileCoordsPyramid.h"
#include "VisiblePlacemark.h"
#include "MathHelper.h"

namespace Marble
{

QVector<GeoDataFeature::GeoDataVisualCategory> sortedVisualCategories()
{
    QVector<GeoDataFeature::GeoDataVisualCategory> visualCategories;

    visualCategories
        << GeoDataFeature::SmallCity
        << GeoDataFeature::SmallCountyCapital  
        << GeoDataFeature::SmallStateCapital   
        << GeoDataFeature::SmallNationCapital
        << GeoDataFeature::MediumCity
        << GeoDataFeature::MediumCountyCapital
        << GeoDataFeature::MediumStateCapital
        << GeoDataFeature::MediumNationCapital
        << GeoDataFeature::BigCity
        << GeoDataFeature::BigCountyCapital
        << GeoDataFeature::BigStateCapital
        << GeoDataFeature::BigNationCapital
        << GeoDataFeature::LargeCity
        << GeoDataFeature::LargeCountyCapital
        << GeoDataFeature::LargeStateCapital
        << GeoDataFeature::LargeNationCapital
        << GeoDataFeature::Nation
        << GeoDataFeature::Mountain
        << GeoDataFeature::Volcano
        << GeoDataFeature::Mons
        << GeoDataFeature::Valley
        << GeoDataFeature::Continent
        << GeoDataFeature::Ocean
        << GeoDataFeature::OtherTerrain
        << GeoDataFeature::Crater
        << GeoDataFeature::Mare
        << GeoDataFeature::GeographicPole
        << GeoDataFeature::MagneticPole
        << GeoDataFeature::ShipWreck
        << GeoDataFeature::AirPort
        << GeoDataFeature::Observatory
        << GeoDataFeature::AccomodationCamping
        << GeoDataFeature::AccomodationHostel
        << GeoDataFeature::AccomodationHotel
        << GeoDataFeature::AccomodationMotel
        << GeoDataFeature::AccomodationYouthHostel
        << GeoDataFeature::AmenityLibrary
        << GeoDataFeature::EducationCollege
        << GeoDataFeature::EducationSchool
        << GeoDataFeature::EducationUniversity
        << GeoDataFeature::FoodBar
        << GeoDataFeature::FoodBiergarten
        << GeoDataFeature::FoodCafe
        << GeoDataFeature::FoodFastFood
        << GeoDataFeature::FoodPub
        << GeoDataFeature::FoodRestaurant
        << GeoDataFeature::HealthDoctors
        << GeoDataFeature::HealthHospital
        << GeoDataFeature::HealthPharmacy
        << GeoDataFeature::MoneyBank
        << GeoDataFeature::ShoppingBeverages
        << GeoDataFeature::ShoppingHifi
        << GeoDataFeature::ShoppingSupermarket
        << GeoDataFeature::TouristAttraction
        << GeoDataFeature::TouristCastle
        << GeoDataFeature::TouristCinema
        << GeoDataFeature::TouristMonument
        << GeoDataFeature::TouristMuseum
        << GeoDataFeature::TouristRuin
        << GeoDataFeature::TouristTheatre
        << GeoDataFeature::TouristThemePark
        << GeoDataFeature::TouristViewPoint
        << GeoDataFeature::TouristZoo
        << GeoDataFeature::TransportAerodrome
        << GeoDataFeature::TransportAirportTerminal
        << GeoDataFeature::TransportBusStation
        << GeoDataFeature::TransportBusStop
        << GeoDataFeature::TransportCarShare
        << GeoDataFeature::TransportFuel
        << GeoDataFeature::TransportParking
        << GeoDataFeature::TransportTrainStation
        << GeoDataFeature::ReligionPlaceOfWorship
        << GeoDataFeature::ReligionBahai
        << GeoDataFeature::ReligionBuddhist
        << GeoDataFeature::ReligionChristian
        << GeoDataFeature::ReligionHindu
        << GeoDataFeature::ReligionJain
        << GeoDataFeature::ReligionJewish
        << GeoDataFeature::ReligionShinto
        << GeoDataFeature::ReligionSikh;

    qSort( visualCategories );

    return visualCategories;
}


PlacemarkLayout::PlacemarkLayout( QAbstractItemModel  *placemarkModel,
                                  QItemSelectionModel *selectionModel,
                                  MarbleClock *clock,
                                  QObject* parent )
    : QObject( parent ),
      m_selectionModel( selectionModel ),
      m_clock( clock ),
      m_acceptedVisualCategories( sortedVisualCategories() ),
      m_showPlaces( false ),
      m_showCities( false ),
      m_showTerrain( false ),
      m_showOtherPlaces( false ),
      m_showLandingSites( false ),
      m_showCraters( false ),
      m_showMaria( false ),
      m_maxLabelHeight( 0 ),
      m_styleResetRequested( true )
{
    m_placemarkModel.setSourceModel( placemarkModel );
    m_placemarkModel.setDynamicSortFilter( true );
    m_placemarkModel.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    m_placemarkModel.sort( 0, Qt::AscendingOrder );

    connect( m_selectionModel,  SIGNAL( selectionChanged( QItemSelection,
                                                           QItemSelection) ),
             this,               SLOT(requestStyleReset()) );

    connect( &m_placemarkModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(resetCacheData()) );
    connect( &m_placemarkModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(addPlacemarks(QModelIndex,int,int)) );
    connect( &m_placemarkModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
             this, SLOT(removePlacemarks(QModelIndex,int,int)) );
    connect( &m_placemarkModel, SIGNAL(modelReset()),
             this, SLOT(resetCacheData()) );
}

PlacemarkLayout::~PlacemarkLayout()
{
    styleReset();
}

void PlacemarkLayout::setShowPlaces( bool show )
{
    m_showPlaces = show;
}

void PlacemarkLayout::setShowCities( bool show )
{
    m_showCities = show;
}

void PlacemarkLayout::setShowTerrain( bool show )
{
    m_showTerrain = show;
}

void PlacemarkLayout::setShowOtherPlaces( bool show )
{
    m_showOtherPlaces = show;
}

void PlacemarkLayout::setShowLandingSites( bool show )
{
    m_showLandingSites = show;
}

void PlacemarkLayout::setShowCraters( bool show )
{
    m_showCraters = show;
}

void PlacemarkLayout::setShowMaria( bool show )
{
    m_showMaria = show;
}

void PlacemarkLayout::requestStyleReset()
{
    mDebug() << "Style reset requested.";
    m_styleResetRequested = true;
}

void PlacemarkLayout::styleReset()
{
    m_paintOrder.clear();
    m_labelArea = 0;
    qDeleteAll( m_visiblePlacemarks );
    m_visiblePlacemarks.clear();
    m_maxLabelHeight = maxLabelHeight();
    m_styleResetRequested = false;
}

QVector<const GeoDataFeature*> PlacemarkLayout::whichPlacemarkAt( const QPoint& curpos )
{
    if ( m_styleResetRequested ) {
        styleReset();
    }

    QVector<const GeoDataFeature*> ret;

    foreach( VisiblePlacemark* mark, m_paintOrder ) {
        if ( mark->labelRect().contains( curpos )
             || QRect( mark->symbolPosition(), mark->symbolPixmap().size() ).contains( curpos ) ) {
            ret.append( mark->placemark() );
        }
    }

    return ret;
}

int PlacemarkLayout::maxLabelHeight() const
{
    int maxLabelHeight = 0;

    for ( int i = 0; i < m_placemarkModel.rowCount(); ++i ) {
        QModelIndex index = m_placemarkModel.index( i, 0 );
        const GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        if ( placemark ) {
            const GeoDataStyle* style = placemark->style();
            QFont labelFont = style->labelStyle().font();
            int textHeight = QFontMetrics( labelFont ).height();
            if ( textHeight > maxLabelHeight )
                maxLabelHeight = textHeight;
        }
    }

    //mDebug() <<"Detected maxLabelHeight: " << maxLabelHeight;
    return maxLabelHeight;
}

/// feed an internal QMap of placemarks with TileId as key when model changes
void PlacemarkLayout::addPlacemarks( QModelIndex parent, int first, int last )
{
    Q_ASSERT( first < m_placemarkModel.rowCount() );
    Q_ASSERT( last < m_placemarkModel.rowCount() );
    for( int i=first; i<=last; ++i ) {
        QModelIndex index = m_placemarkModel.index( i, 0, parent );
        Q_ASSERT( index.isValid() );
        const GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );
        if ( !coordinates.isValid() ) {
            continue;
        }

        int zoomLevel = placemark->zoomLevel();
        TileId key = TileId::fromCoordinates( coordinates, zoomLevel );
        m_placemarkCache[key].append( placemark );
    }
    requestStyleReset();
    emit repaintNeeded();
}

void PlacemarkLayout::removePlacemarks( QModelIndex parent, int first, int last )
{
    Q_ASSERT( first < m_placemarkModel.rowCount() );
    Q_ASSERT( last < m_placemarkModel.rowCount() );
    for( int i=first; i<=last; ++i ) {
        QModelIndex index = m_placemarkModel.index( i, 0, parent );
        Q_ASSERT( index.isValid() );
        const GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );
        if ( !coordinates.isValid() ) {
            continue;
        }

        int zoomLevel = placemark->zoomLevel();
        TileId key = TileId::fromCoordinates( coordinates, zoomLevel );
        m_placemarkCache[key].removeAll( placemark );
    }
    emit repaintNeeded();
}

void PlacemarkLayout::resetCacheData()
{
    const int rowCount = m_placemarkModel.rowCount();

    m_placemarkCache.clear();
    requestStyleReset();
    addPlacemarks( m_placemarkModel.index( 0, 0 ), 0, rowCount );
    emit repaintNeeded();
}

QSet<TileId> PlacemarkLayout::visibleTiles( const ViewportParams *viewport )
{
    int zoomLevel = qLn( viewport->radius() *4 / 256 ) / qLn( 2.0 );

    /*
     * rely on m_placemarkCache to find the placemarks for the tiles which
     * matter. The top level tiles have the more popular placemarks,
     * the bottom level tiles have the smaller ones, and we only get the ones
     * matching our latLonAltBox.
     */

    qreal north, south, east, west;
    viewport->viewLatLonAltBox().boundaries(north, south, east, west);
    QSet<TileId> tileIdSet;
    QVector<QRectF> geoRects;
    if( west <= east ) {
        geoRects << QRectF(west, north, east - west, south - north);
    } else {
        geoRects << QRectF(west, north, M_PI - west, south - north);
        geoRects << QRectF(-M_PI, north, east + M_PI, south - north);
    }
    foreach( const QRectF &geoRect, geoRects ) {
        TileId key;
        QRect rect;

        key = TileId::fromCoordinates( GeoDataCoordinates(geoRect.left(), north, 0), zoomLevel);
        rect.setLeft( key.x() );
        rect.setTop( key.y() );

        key = TileId::fromCoordinates( GeoDataCoordinates(geoRect.right(), south, 0), zoomLevel);
        rect.setRight( key.x() );
        rect.setBottom( key.y() );

        TileCoordsPyramid pyramid(0, zoomLevel );
        pyramid.setBottomLevelCoords( rect );

        for ( int level = pyramid.topLevel(); level <= pyramid.bottomLevel(); ++level ) {
        QRect const coords = pyramid.coords( level );
        int x1, y1, x2, y2;
        coords.getCoords( &x1, &y1, &x2, &y2 );
            for ( int x = x1; x <= x2; ++x ) {
                for ( int y = y1; y <= y2; ++y ) {
                    TileId const tileId( 0, level, x, y );
                    tileIdSet.insert(tileId);
                }
            }
        }
    }

    return tileIdSet;
}

QVector<VisiblePlacemark *> PlacemarkLayout::generateLayout( const ViewportParams *viewport )
{
    m_runtimeTrace.clear();
    if ( m_placemarkModel.rowCount() <= 0 )
        return QVector<VisiblePlacemark *>();

    if ( m_styleResetRequested ) {
        styleReset();
    }

    if ( m_maxLabelHeight == 0 ) {
        return QVector<VisiblePlacemark *>();
    }

    const int secnumber = viewport->height() / m_maxLabelHeight + 1;
    m_rowsection.clear();
    m_rowsection.resize(secnumber);

    m_paintOrder.clear();
    m_labelArea = 0;

    // First handle the selected placemarks as they have the highest priority.

    const QModelIndexList selectedIndexes = m_selectionModel->selection().indexes();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );
        const GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        Q_ASSERT(placemark);
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );

        if ( !coordinates.isValid() ) {
            continue;
        }

        qreal x = 0;
        qreal y = 0;

        if ( !viewport->viewLatLonAltBox().contains( coordinates ) ||
             ! viewport->screenCoordinates( coordinates, x, y ))
            {
                delete m_visiblePlacemarks.take( placemark );
                continue;
            }

        if( layoutPlacemark( placemark, x, y, true) ) {
            // Make sure not to draw more placemarks on the screen than
            // specified by placemarksOnScreenLimit().
            if ( placemarksOnScreenLimit( viewport->size() ) )
                break;
        }

    }

    // Now handle all other placemarks...

    const QItemSelection selection = m_selectionModel->selection();

    QList<TileId> tileIdList = visibleTiles( viewport ).toList();
    qSort( tileIdList );
    QList<const GeoDataPlacemark*> placemarkList;
    foreach ( const TileId &tileId, tileIdList ) {
        placemarkList += m_placemarkCache.value( tileId );
    }

    foreach ( const GeoDataPlacemark *placemark, placemarkList ) {
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );
        if ( !coordinates.isValid() ) {
            continue;
        }

        int zoomLevel = placemark->zoomLevel();
        if ( zoomLevel > 18 ) {
            break;
        }

        qreal x = 0;
        qreal y = 0;

        if ( !viewport->viewLatLonAltBox().contains( coordinates ) ||
             ! viewport->screenCoordinates( coordinates, x, y )) {
                delete m_visiblePlacemarks.take( placemark );
                continue;
            }

        if ( !placemark->isGloballyVisible() ) {
            continue;
        }

        const GeoDataFeature::GeoDataVisualCategory visualCategory = placemark->visualCategory();

        // Skip city marks if we're not showing cities.
        if ( !m_showCities
             && visualCategory >= GeoDataFeature::SmallCity
             && visualCategory <= GeoDataFeature::Nation )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( !m_showTerrain
             && visualCategory >= GeoDataFeature::Mountain
             && visualCategory <= GeoDataFeature::OtherTerrain )
            continue;

        // Skip other places if we're not showing other places.
        if ( !m_showOtherPlaces
             && visualCategory >= GeoDataFeature::GeographicPole
             && visualCategory <= GeoDataFeature::Observatory )
            continue;

        // Skip landing sites if we're not showing landing sites.
        if ( !m_showLandingSites
             && visualCategory >= GeoDataFeature::MannedLandingSite
             && visualCategory <= GeoDataFeature::UnmannedHardLandingSite )
            continue;

        // Skip craters if we're not showing craters.
        if ( !m_showCraters
             && visualCategory == GeoDataFeature::Crater )
            continue;

        // Skip maria if we're not showing maria.
        if ( !m_showMaria
             && visualCategory == GeoDataFeature::Mare )
            continue;

        if ( !m_showPlaces
             && visualCategory >= GeoDataFeature::GeographicPole
             && visualCategory <= GeoDataFeature::Observatory )
            continue;

        // We handled selected placemarks already, so we skip them here...
        // Assuming that only a small amount of places is selected
        // we check for the selected state after all other filters
        bool isSelected = false;
        foreach ( const QModelIndex &index, selection.indexes() ) {
            const GeoDataPlacemark *mark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
            if (mark == placemark ) {
                isSelected = true;
                break;
            }
        }
        if ( isSelected )
            continue;

        if( layoutPlacemark( placemark, x, y, isSelected ) ) {
            // Make sure not to draw more placemarks on the screen than
            // specified by placemarksOnScreenLimit().
            if ( placemarksOnScreenLimit( viewport->size() ) )
                break;
        }
    }

    m_runtimeTrace = QString("Placemarks: %1 Drawn: %2").arg( placemarkList.count() ).arg( m_paintOrder.size() );
    return m_paintOrder;
}

QString PlacemarkLayout::runtimeTrace() const
{
    return m_runtimeTrace;
}

bool PlacemarkLayout::layoutPlacemark( const GeoDataPlacemark *placemark, qreal x, qreal y, bool selected )
{
    // Choose Section

    // Find out whether the area around the placemark is covered already.
    // If there's not enough space free don't add a VisiblePlacemark here.
    const GeoDataStyle* style = placemark->style();

    QRectF labelRect;
    if( !placemark->name().isEmpty() ) {
        labelRect = roomForLabel( style, x, y, placemark->name() );
        if ( labelRect.isNull() ) {
            return false;
        }
    }

    // Find the corresponding visible placemark
    VisiblePlacemark *mark = m_visiblePlacemarks.value( placemark );
    if ( !mark ) {
        // If there is no visible placemark yet for this index,
        // create a new one...
        mark = new VisiblePlacemark( placemark );
        m_visiblePlacemarks.insert( placemark, mark );
        connect( mark, SIGNAL(updateNeeded()), this, SIGNAL(repaintNeeded()) );
    }

    // Finally save the label position on the map.
    QPointF hotSpot = mark->hotSpot();

    if( mark->selected() != selected ) {
        mark->setSelected( selected );
    }
    mark->setSymbolPosition( QPoint( x - qRound( hotSpot.x() ),
                                     y - qRound( hotSpot.y() ) ) );
    mark->setLabelRect( labelRect );

    if ( !labelRect.isEmpty() ) {
        // Add the current placemark to the matching row and its
        // direct neighbors.
        int idx = y / m_maxLabelHeight;
        if ( idx - 1 >= 0 )
            m_rowsection[ idx - 1 ].append( mark );
        m_rowsection[ idx ].append( mark );
        if ( idx + 1 < m_rowsection.size() )
            m_rowsection[ idx + 1 ].append( mark );
    }

    m_paintOrder.append( mark );
    m_labelArea += labelRect.width() * labelRect.height();
    return true;
}

GeoDataCoordinates PlacemarkLayout::placemarkIconCoordinates( const GeoDataPlacemark *placemark ) const
{
    bool ok;
    GeoDataCoordinates coordinates = placemark->coordinate( m_clock->dateTime(), &ok );
    if ( !ok && qBinaryFind( m_acceptedVisualCategories, placemark->visualCategory() )
                != m_acceptedVisualCategories.constEnd() ) {
        ok = true;
    }

    if ( ok ) {
        return coordinates;
    }

    return GeoDataCoordinates();
}

QRectF PlacemarkLayout::roomForLabel( const GeoDataStyle * style,
                                      const qreal x, const qreal y,
                                      const QString &labelText ) const
{
    QFont labelFont = style->labelStyle().font();
    int textHeight = QFontMetrics( labelFont ).height();

    int textWidth;
    if ( style->labelStyle().glow() ) {
        labelFont.setWeight( 75 ); // Needed to calculate the correct pixmap size;
        textWidth = ( QFontMetrics( labelFont ).width( labelText )
            + qRound( 2 * s_labelOutlineWidth ) );
    } else {
        textWidth = ( QFontMetrics( labelFont ).width( labelText ) );
    }

    const QVector<VisiblePlacemark*> currentsec = m_rowsection.at( y / m_maxLabelHeight );

    if ( style->labelStyle().alignment() == GeoDataLabelStyle::Corner ) {
        const int symbolWidth = style->iconStyle().icon().width();

        // Check the four possible positions by going through all of them
        for( int i=0; i<4; ++i ) {
            const qreal xPos = ( i/2 == 0 ) ? x + symbolWidth / 2 + 1 :
                                              x - symbolWidth / 2 - 1 - textWidth;
            const qreal yPos = ( i%2 == 0 ) ? y :
                                              y - textHeight;
            const QRectF labelRect = QRectF( xPos, yPos, textWidth, textHeight );

            // Check if there is another label or symbol that overlaps.
            bool isRoom = true;
            QVector<VisiblePlacemark*>::const_iterator beforeItEnd = currentsec.constEnd();
            for ( QVector<VisiblePlacemark*>::ConstIterator beforeIt = currentsec.constBegin();
                  beforeIt != beforeItEnd;
                  ++beforeIt ) {
                if ( labelRect.intersects( (*beforeIt)->labelRect()) ) {
                    isRoom = false;
                    break;
                }
            }

            if ( isRoom ) {
                // claim the place immediately if it hasn't been used yet
                return labelRect;
            }
        }
    }
    else if ( style->labelStyle().alignment() == GeoDataLabelStyle::Center ) {
        bool isRoom = true;
        QRectF  labelRect( x - textWidth / 2, y - textHeight / 2,
                          textWidth, textHeight );

        // Check if there is another label or symbol that overlaps.
        QVector<VisiblePlacemark*>::const_iterator beforeItEnd = currentsec.constEnd();
        for ( QVector<VisiblePlacemark*>::ConstIterator beforeIt = currentsec.constBegin();
              beforeIt != beforeItEnd; ++beforeIt )
        {
            if ( labelRect.intersects( (*beforeIt)->labelRect() ) ) {
                isRoom = false;
                break;
            }
        }

        if ( isRoom ) {
            // claim the place immediately if it hasn't been used yet 
            return labelRect;
        }
    }

    return QRectF(); // At this point there is no space left
                     // for the rectangle anymore.
}

bool PlacemarkLayout::placemarksOnScreenLimit( const QSize &screenSize ) const
{
    int ratio = ( m_labelArea * 100 ) / ( screenSize.width() * screenSize.height() );
    return ratio >= 40;
}

}

#include "PlacemarkLayout.moc"
