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
#include <QVectorIterator>
#include <QFont>
#include <QFontMetrics>
#include <QItemSelectionModel>
#include <qmath.h>

#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataTypes.h"
#include "OsmPlacemarkData.h"

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
#include <StyleBuilder.h>

namespace
{   //Helper function that checks for available room for the label
    bool hasRoomFor(const QVector<Marble::VisiblePlacemark*> & placemarks, const QRectF &boundingBox)
    {
        // Check if there is another label or symbol that overlaps.
        QVector<Marble::VisiblePlacemark*>::const_iterator beforeItEnd = placemarks.constEnd();
        for ( QVector<Marble::VisiblePlacemark*>::ConstIterator beforeIt = placemarks.constBegin();
              beforeIt != beforeItEnd; ++beforeIt )
        {
            if ( boundingBox.intersects( (*beforeIt)->boundingBox() ) ) {
                return false;
            }
        }
        return true;
    }
}

namespace Marble
{

QSet<GeoDataPlacemark::GeoDataVisualCategory> acceptedVisualCategories()
{
    QSet<GeoDataPlacemark::GeoDataVisualCategory> visualCategories;

    visualCategories
        << GeoDataPlacemark::SmallCity
        << GeoDataPlacemark::SmallCountyCapital
        << GeoDataPlacemark::SmallStateCapital
        << GeoDataPlacemark::SmallNationCapital
        << GeoDataPlacemark::MediumCity
        << GeoDataPlacemark::MediumCountyCapital
        << GeoDataPlacemark::MediumStateCapital
        << GeoDataPlacemark::MediumNationCapital
        << GeoDataPlacemark::BigCity
        << GeoDataPlacemark::BigCountyCapital
        << GeoDataPlacemark::BigStateCapital
        << GeoDataPlacemark::BigNationCapital
        << GeoDataPlacemark::LargeCity
        << GeoDataPlacemark::LargeCountyCapital
        << GeoDataPlacemark::LargeStateCapital
        << GeoDataPlacemark::LargeNationCapital
        << GeoDataPlacemark::Nation
        << GeoDataPlacemark::Mountain
        << GeoDataPlacemark::Volcano
        << GeoDataPlacemark::Mons
        << GeoDataPlacemark::Valley
        << GeoDataPlacemark::Continent
        << GeoDataPlacemark::Ocean
        << GeoDataPlacemark::OtherTerrain
        << GeoDataPlacemark::Crater
        << GeoDataPlacemark::Mare
        << GeoDataPlacemark::GeographicPole
        << GeoDataPlacemark::MagneticPole
        << GeoDataPlacemark::ShipWreck
        << GeoDataPlacemark::PlaceSuburb
        << GeoDataPlacemark::PlaceHamlet
        << GeoDataPlacemark::PlaceLocality;

    return visualCategories;
}


PlacemarkLayout::PlacemarkLayout( QAbstractItemModel  *placemarkModel,
                                  QItemSelectionModel *selectionModel,
                                  MarbleClock *clock,
                                  const StyleBuilder *styleBuilder,
                                  QObject* parent )
    : QObject( parent ),
      m_placemarkModel(placemarkModel),
      m_selectionModel( selectionModel ),
      m_clock( clock ),
      m_acceptedVisualCategories( acceptedVisualCategories() ),
      m_showPlaces( false ),
      m_showCities( false ),
      m_showTerrain( false ),
      m_showOtherPlaces( false ),
      m_showLandingSites( false ),
      m_showCraters( false ),
      m_showMaria( false ),
      m_maxLabelHeight(maxLabelHeight()),
      m_styleResetRequested( true ),
      m_styleBuilder(styleBuilder)
{
    Q_ASSERT(m_placemarkModel);

    connect( m_selectionModel,  SIGNAL( selectionChanged( QItemSelection,
                                                           QItemSelection) ),
             this,               SLOT(requestStyleReset()) );

    connect( m_placemarkModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(resetCacheData()) );
    connect( m_placemarkModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(addPlacemarks(QModelIndex,int,int)) );
    connect( m_placemarkModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
             this, SLOT(removePlacemarks(QModelIndex,int,int)) );
    connect( m_placemarkModel, SIGNAL(modelReset()),
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
        if ( mark->labelRect().contains( curpos ) || mark->symbolRect().contains( curpos ) ) {
            ret.append( mark->placemark() );
        }
    }

    return ret;
}

int PlacemarkLayout::maxLabelHeight() const
{
    QFont const standardFont(QStringLiteral("Sans Serif"));
    return QFontMetrics(standardFont).height();
}

/// feed an internal QMap of placemarks with TileId as key when model changes
void PlacemarkLayout::addPlacemarks( const QModelIndex& parent, int first, int last )
{
    Q_ASSERT( first < m_placemarkModel->rowCount() );
    Q_ASSERT( last < m_placemarkModel->rowCount() );
    for( int i=first; i<=last; ++i ) {
        QModelIndex index = m_placemarkModel->index( i, 0, parent );
        Q_ASSERT( index.isValid() );
        const GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );
        if ( !coordinates.isValid() ) {
            continue;
        }

        if (placemark->hasOsmData()) {
            qint64 const osmId = placemark->osmData().id();
            if (osmId > 0) {
                if (m_osmIds.contains(osmId)) {
                    continue; // placemark is already shown
                }
                m_osmIds << osmId;
            }
        }

        int zoomLevel = placemark->zoomLevel();
        TileId key = TileId::fromCoordinates( coordinates, zoomLevel );
        m_placemarkCache[key].append( placemark );
    }
    emit repaintNeeded();
}

void PlacemarkLayout::removePlacemarks( const QModelIndex& parent, int first, int last )
{
    Q_ASSERT( first < m_placemarkModel->rowCount() );
    Q_ASSERT( last < m_placemarkModel->rowCount() );
    for( int i=first; i<=last; ++i ) {
        QModelIndex index = m_placemarkModel->index( i, 0, parent );
        Q_ASSERT( index.isValid() );
        const GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );
        if ( !coordinates.isValid() ) {
            continue;
        }

        int zoomLevel = placemark->zoomLevel();
        TileId key = TileId::fromCoordinates( coordinates, zoomLevel );
        m_placemarkCache[key].removeAll( placemark );
        if (placemark->hasOsmData()) {
            qint64 const osmId = placemark->osmData().id();
            if (osmId > 0) {
                m_osmIds.remove(osmId);
            }
        }
    }
    emit repaintNeeded();
}

void PlacemarkLayout::resetCacheData()
{
    const int rowCount = m_placemarkModel->rowCount();

    m_osmIds.clear();
    m_placemarkCache.clear();
    requestStyleReset();
    addPlacemarks( m_placemarkModel->index( 0, 0 ), 0, rowCount );
    emit repaintNeeded();
}

QSet<TileId> PlacemarkLayout::visibleTiles( const ViewportParams *viewport )
{
    int zoomLevel = qLn( viewport->radius()/64.0 ) / qLn( 2.0 );

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
    if ( m_placemarkModel->rowCount() <= 0 )
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
    auto const viewLatLonAltBox = viewport->viewLatLonAltBox();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );
        const GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );

        if ( !coordinates.isValid() ) {
            continue;
        }

        qreal x = 0;
        qreal y = 0;

        if ( !viewLatLonAltBox.contains( coordinates ) ||
             ! viewport->screenCoordinates( coordinates, x, y ))
            {
                continue;
            }

        if( layoutPlacemark( placemark, coordinates, x, y, true) ) {
            // Make sure not to draw more placemarks on the screen than
            // specified by placemarksOnScreenLimit().
            if ( placemarksOnScreenLimit( viewport->size() ) )
                break;
        }

    }

    // Now handle all other placemarks...

    const QItemSelection selection = m_selectionModel->selection();

    QList<const GeoDataPlacemark*> placemarkList;
    foreach ( const TileId &tileId, visibleTiles( viewport ) ) {
        placemarkList += m_placemarkCache.value( tileId );
    }
    qSort(placemarkList.begin(), placemarkList.end(), GeoDataPlacemark::placemarkLayoutOrderCompare);

    foreach ( const GeoDataPlacemark *placemark, placemarkList ) {
        const GeoDataCoordinates coordinates = placemarkIconCoordinates( placemark );
        if ( !coordinates.isValid() ) {
            continue;
        }

        int zoomLevel = placemark->zoomLevel();
        if ( zoomLevel > 20 ) {
            break;
        }

        qreal x = 0;
        qreal y = 0;

        if ( !viewLatLonAltBox.contains( coordinates ) ||
             ! viewport->screenCoordinates( coordinates, x, y )) {
                continue;
            }

        if ( !placemark->isGloballyVisible() ) {
            continue;
        }

        const GeoDataPlacemark::GeoDataVisualCategory visualCategory = placemark->visualCategory();

        // Skip city marks if we're not showing cities.
        if ( !m_showCities
             && visualCategory >= GeoDataPlacemark::SmallCity
             && visualCategory <= GeoDataPlacemark::Nation )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( !m_showTerrain
             && visualCategory >= GeoDataPlacemark::Mountain
             && visualCategory <= GeoDataPlacemark::OtherTerrain )
            continue;

        // Skip other places if we're not showing other places.
        if ( !m_showOtherPlaces
             && visualCategory >= GeoDataPlacemark::GeographicPole
             && visualCategory <= GeoDataPlacemark::Observatory )
            continue;

        // Skip landing sites if we're not showing landing sites.
        if ( !m_showLandingSites
             && visualCategory >= GeoDataPlacemark::MannedLandingSite
             && visualCategory <= GeoDataPlacemark::UnmannedHardLandingSite )
            continue;

        // Skip craters if we're not showing craters.
        if ( !m_showCraters
             && visualCategory == GeoDataPlacemark::Crater )
            continue;

        // Skip maria if we're not showing maria.
        if ( !m_showMaria
             && visualCategory == GeoDataPlacemark::Mare )
            continue;

        if ( !m_showPlaces
             && visualCategory >= GeoDataPlacemark::GeographicPole
             && visualCategory <= GeoDataPlacemark::Observatory )
            continue;

        // We handled selected placemarks already, so we skip them here...
        // Assuming that only a small amount of places is selected
        // we check for the selected state after all other filters
        bool isSelected = false;
        foreach ( const QModelIndex &index, selection.indexes() ) {
            const GeoDataPlacemark *mark = static_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
            if (mark == placemark ) {
                isSelected = true;
                break;
            }
        }
        if ( isSelected )
            continue;

        if( layoutPlacemark( placemark, coordinates, x, y, isSelected ) ) {
            // Make sure not to draw more placemarks on the screen than
            // specified by placemarksOnScreenLimit().
            if ( placemarksOnScreenLimit( viewport->size() ) )
                break;
        }
    }

    if (m_visiblePlacemarks.size() > qMax(100, 4 * m_paintOrder.size())) {
        auto const extendedBox = viewLatLonAltBox.scaled(2.0, 2.0);
        QVector<VisiblePlacemark*> outdated;
        for (auto placemark: m_visiblePlacemarks) {
            if (!extendedBox.contains(placemark->coordinates())) {
                outdated << placemark;
            }
        }
        for (auto placemark: outdated) {
            delete m_visiblePlacemarks.take(placemark->placemark());
        }
    }

    m_runtimeTrace = QStringLiteral("Placemarks: %1 Drawn: %2").arg(placemarkList.count()).arg(m_paintOrder.size());
    return m_paintOrder;
}

QString PlacemarkLayout::runtimeTrace() const
{
    return m_runtimeTrace;
}

QList<VisiblePlacemark *> PlacemarkLayout::visiblePlacemarks() const
{
    return m_visiblePlacemarks.values();
}

bool PlacemarkLayout::layoutPlacemark( const GeoDataPlacemark *placemark, const GeoDataCoordinates &coordinates, qreal x, qreal y, bool selected )
{
    // Find the corresponding visible placemark
    VisiblePlacemark *mark = m_visiblePlacemarks.value( placemark );
    if ( !mark ) {
        // If there is no visible placemark yet for this index,
        // create a new one...
        StyleParameters parameters;
        // @todo: Set / adjust to tile level
        parameters.placemark = placemark;

        auto style = m_styleBuilder->createStyle(parameters);
        mark = new VisiblePlacemark(placemark, coordinates, style);
        m_visiblePlacemarks.insert( placemark, mark );
        connect( mark, SIGNAL(updateNeeded()), this, SIGNAL(repaintNeeded()) );
    }
    GeoDataStyle::ConstPtr style = mark->style();

    // Choose Section

    QPointF hotSpot = mark->hotSpot();
    mark->setSelected(selected);
    mark->setSymbolPosition(QPointF(x - hotSpot.x(), y - hotSpot.y()));

    // Find out whether the area around the placemark is covered already.
    // If there's not enough space free don't add a VisiblePlacemark here.

    QRectF labelRect;

    const QString labelText = placemark->displayName();
    if (!labelText.isEmpty()) {
        labelRect = roomForLabel(style, x, y, labelText, mark);
    }
    if (labelRect.isNull() && (mark->symbolPixmap().isNull() || !hasRoomForPixmap(y, mark))) {
        return false;
    }

    mark->setLabelRect( labelRect );
    // Add the current placemark to the matching row and its
    // direct neighbors.
    int idx = y / m_maxLabelHeight;
    if ( idx - 1 >= 0 ) {
        m_rowsection[ idx - 1 ].append( mark );
    }
    m_rowsection[ idx ].append( mark );
    if ( idx + 1 < m_rowsection.size() ) {
        m_rowsection[ idx + 1 ].append( mark );
    }

    m_paintOrder.append( mark );
    QRectF const boundingBox = mark->boundingBox();
    Q_ASSERT(!boundingBox.isEmpty());
    m_labelArea += boundingBox.width() * boundingBox.height();
    m_maxLabelHeight = qMax(m_maxLabelHeight, qCeil(boundingBox.height()));
    return true;
}

GeoDataCoordinates PlacemarkLayout::placemarkIconCoordinates( const GeoDataPlacemark *placemark ) const
{
    GeoDataCoordinates coordinates = placemark->coordinate( m_clock->dateTime());
    if (!m_acceptedVisualCategories.contains(placemark->visualCategory())) {
        StyleParameters parameters;
        parameters.placemark = placemark;
        auto style = m_styleBuilder->createStyle(parameters);
        if (style->iconStyle().icon().isNull()) {
            return GeoDataCoordinates();
        }
    }

    return coordinates;
}

QRectF PlacemarkLayout::roomForLabel( const GeoDataStyle::ConstPtr &style,
                                      const qreal x, const qreal y,
                                      const QString &labelText,
                                      const VisiblePlacemark* placemark) const
{
    QFont labelFont = style->labelStyle().scaledFont();
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
    QRectF const symbolRect = placemark->symbolRect();

    if ( style->labelStyle().alignment() == GeoDataLabelStyle::Corner ) {
        const int symbolWidth = style->iconStyle().scaledIcon().size().width();

        // Check the four possible positions by going through all of them
        for( int i=0; i<4; ++i ) {
            const qreal xPos = ( i/2 == 0 ) ? x + symbolWidth / 2 + 1 :
                                              x - symbolWidth / 2 - 1 - textWidth;
            const qreal yPos = ( i%2 == 0 ) ? y :
                                              y - textHeight;
            const QRectF labelRect = QRectF( xPos, yPos, textWidth, textHeight );

            if (hasRoomFor(currentsec, labelRect.united(symbolRect))) {
                // claim the place immediately if it hasn't been used yet
                return labelRect;
            }
        }
    }
    else if ( style->labelStyle().alignment() == GeoDataLabelStyle::Center ) {
        int const offsetY = style->iconStyle().scaledIcon().height() / 2.0;
        QRectF  labelRect = QRectF( x - textWidth / 2, y - offsetY - textHeight,
                          textWidth, textHeight );

        if (hasRoomFor(currentsec, labelRect.united(symbolRect))) {
            // claim the place immediately if it hasn't been used yet 
            return labelRect;
        }
    }
    else if (style->labelStyle().alignment() == GeoDataLabelStyle::Right)
    {
        const int symbolWidth = style->iconStyle().icon().width();
        const qreal startY = y - textHeight/2;
        const qreal xPos = x + symbolWidth / 2 + 1;

        // Check up to seven vertical positions (center, +3, -3 from center)
        for(int i=0; i<7; ++i)
        {
            const qreal increase = (i/2) * (textHeight + 1); //intentional integer arithmetics
            const qreal direction = (i%2 == 0 ? 1 : -1);
            const qreal yPos = startY + increase*direction;

            const QRectF labelRect = QRectF(xPos, yPos, textWidth, textHeight);

            if (hasRoomFor(currentsec, labelRect.united(symbolRect)))
            {
                return labelRect;
            }
        }
    }

    // At this point there is no space left for the rectangle anymore.
    return QRectF();
}

bool PlacemarkLayout::hasRoomForPixmap(const qreal y, const VisiblePlacemark *placemark) const
{
    const QVector<VisiblePlacemark*> currentsec = m_rowsection.at(y / m_maxLabelHeight);
    return hasRoomFor(currentsec, placemark->symbolRect());
}

bool PlacemarkLayout::placemarksOnScreenLimit( const QSize &screenSize ) const
{
    int ratio = ( m_labelArea * 100 ) / ( screenSize.width() * screenSize.height() );
    return ratio >= 40;
}

}

#include "moc_PlacemarkLayout.cpp"
