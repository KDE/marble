//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//

#include "PlacemarkLayout.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QVector>
#include <QtCore/QVectorIterator>
#include <QtGui/QFont>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPainter>

#include "GeoSceneDocument.h"
#include "GeoSceneMap.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"

#include "MarbleDebug.h"
#include "global.h"
#include "PlacemarkPainter.h"
#include "MarblePlacemarkModel.h"
#include "MarbleDirs.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "AbstractProjection.h"
#include "VisiblePlacemark.h"
#include "MathHelper.h"

using namespace Marble;

PlacemarkLayout::PlacemarkLayout( const QAbstractItemModel  *placemarkModel,
                                  const QItemSelectionModel *selectionModel,
                                  QObject* parent )
    : QObject( parent ),
      m_placemarkModel( placemarkModel ),
      m_selectionModel( selectionModel ),
      m_placemarkPainter( 0 ),
      m_maxLabelHeight( 0 ),
      m_styleResetRequested( true )
{
//  Old weightfilter array. Still here 
// to be able to compare performance
/*
    m_weightfilter
        << 9999
        << 4200
        << 3900
        << 3600

        << 3300
        << 3000
        << 2700
        << 2400

        << 2100
        << 1800
        << 1500
        << 1200

        << 900
        << 400
        << 200
        << 0;
*/

// lower radius limit, level
    m_weightfilter  
        << 49300    // 0
        << 40300    // 1
        << 32300    // 2
        << 25300    // 3

        << 19300    // 4
        << 14300    // 5
        << 10300    // 6
        << 7300     // 7

        << 5300     // 8
        << 3300     // 9
        << 2400     // 10
        << 1800     // 11

        << 1200     // 12
        << 800      // 13
        << 300      // 14
        << 250      // 15

        << 200      // 16
        << 150      // 17
        << 100      // 18
        << 50       // 19

        << 0;       // 20

    m_placemarkPainter =  new PlacemarkPainter( this );
}

PlacemarkLayout::~PlacemarkLayout()
{
    styleReset();
}

void PlacemarkLayout::requestStyleReset()
{
    mDebug() << "Style reset requested.";
    m_styleResetRequested = true;
}

void PlacemarkLayout::styleReset()
{
    m_paintOrder.clear();
    qDeleteAll( m_visiblePlacemarks );
    m_visiblePlacemarks.clear();
}

QVector<QModelIndex> PlacemarkLayout::whichPlacemarkAt( const QPoint& curpos )
{
    if ( m_styleResetRequested ) {
        styleReset();
    }

    QVector<QModelIndex> ret;

    QVector<VisiblePlacemark*>::ConstIterator  it;
    QVector<VisiblePlacemark*>::ConstIterator  itEnd = m_paintOrder.constEnd();
    for ( it = m_paintOrder.constBegin();
          it != itEnd; ++it )
    {
        const VisiblePlacemark  *mark = *it; // no cast

        if ( mark->labelRect().contains( curpos )
             || QRect( mark->symbolPosition(), mark->symbolPixmap().size() ).contains( curpos ) ) {
            ret.append( mark->modelIndex() );
        }
    }

    return ret;
}

int PlacemarkLayout::maxLabelHeight() const
{
    mDebug() << "Detecting maxLabelHeight ...";

    int maxLabelHeight = 0;

    const QModelIndexList selectedIndexes = m_selectionModel->selection().indexes();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );
        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        GeoDataStyle* style = placemark->style();
        QFont labelFont = style->labelStyle().font();
        int textHeight = QFontMetrics( labelFont ).height();
        if ( textHeight > maxLabelHeight )
            maxLabelHeight = textHeight; 
    }

    for ( int i = 0; i < m_placemarkModel->rowCount(); ++i ) {
        QModelIndex index = m_placemarkModel->index( i, 0 );
        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        if ( placemark ) {
            GeoDataStyle* style = placemark->style();
            QFont labelFont = style->labelStyle().font();
            int textHeight = QFontMetrics( labelFont ).height();
            if ( textHeight > maxLabelHeight )
                maxLabelHeight = textHeight;
        }
    }

    //mDebug() <<"Detected maxLabelHeight: " << maxLabelHeight;
    return maxLabelHeight;
}

void PlacemarkLayout::paintPlaceFolder( QPainter   *painter,
                                        ViewParams *viewParams )
{
    if ( m_placemarkModel->rowCount() <= 0 )
        return;

    // const int imgwidth  = viewParams->canvasImage()->width();
    const int imgheight = viewParams->canvasImage()->height();

    if ( m_styleResetRequested ) {
        m_styleResetRequested = false;
        styleReset();

        m_maxLabelHeight = maxLabelHeight();
    }
    const int   secnumber         = imgheight / m_maxLabelHeight + 1;

    //Quaternion  inversePlanetAxis = viewParams->m_planetAxis.inverse();

    QVector< QVector< VisiblePlacemark* > >  rowsection;
    for ( int i = 0; i < secnumber; i++)
        rowsection.append( QVector<VisiblePlacemark*>( ) );

    m_paintOrder.clear();

    int labelnum = 0;
    qreal x = 0;
    qreal y = 0;

    // earth
    bool showPlaces, showCities, showTerrain, showOtherPlaces;

    viewParams->propertyValue( "places", showPlaces );
    viewParams->propertyValue( "cities", showCities );
    viewParams->propertyValue( "terrain", showTerrain );
    viewParams->propertyValue( "otherplaces", showOtherPlaces );
    
    // other planets
    bool showLandingSites, showCraters, showMaria;

    viewParams->propertyValue( "landingsites", showLandingSites );
    viewParams->propertyValue( "craters", showCraters );
    viewParams->propertyValue( "maria", showMaria );

    GeoDataLatLonAltBox latLonAltBox = viewParams->viewport()->viewLatLonAltBox();

    /**
     * First handle the selected placemarks, as they have the highest priority.
     */

    const QModelIndexList selectedIndexes = m_selectionModel->selection().indexes();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );
        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        GeoDataGeometry *geometry = placemark->geometry();
        if( !dynamic_cast<GeoDataPoint*>(geometry) ) {
            continue;
        }

        GeoDataCoordinates geopoint = placemark->coordinate();

        if ( !latLonAltBox.contains( geopoint ) ||
             ! viewParams->currentProjection()->screenCoordinates( geopoint, viewParams->viewport(), x, y ))
            {
                delete m_visiblePlacemarks.take( index );
                continue;
            }

        // ----------------------------------------------------------------
        // End of checks. Here the actual layouting starts.
        int textWidth = 0;

        // Find the corresponding visible placemark
        VisiblePlacemark *mark = m_visiblePlacemarks.value( index );

        GeoDataStyle* style = placemark->style();

        // Specify font properties
        if ( mark ) {
            textWidth = mark->labelRect().width();
        }
        else {
            QFont labelFont = style->labelStyle().font();
            labelFont.setWeight( 75 ); // Needed to calculate the correct pixmap size; 

            textWidth = ( QFontMetrics( labelFont ).width( placemark->name() )
                  + (int)( 2 * s_labelOutlineWidth ) );
        }

        // Choose Section
        const QVector<VisiblePlacemark*> currentsec = rowsection.at( y / m_maxLabelHeight );

        // Find out whether the area around the placemark is covered already.
        // If there's not enough space free don't add a VisiblePlacemark here.

        QRect labelRect = roomForLabel( style, currentsec, x, y, textWidth );
        if ( labelRect.isNull() )
            continue;

        // Make sure not to draw more placemarks on the screen than 
        // specified by placemarksOnScreenLimit().

        ++labelnum;
        if ( labelnum >= placemarksOnScreenLimit() )
            break;
        if ( !mark ) {
            // If there is no visible placemark yet for this index,
            // create a new one...
            mark = new VisiblePlacemark;
            mark->setModelIndex( QModelIndex( index ) );

            m_visiblePlacemarks.insert( index, mark );
        }

        // Finally save the label position on the map.
        QPointF hotSpot = style->iconStyle().hotSpot();

        mark->setSymbolPosition( QPoint( x - (int)( hotSpot.x() ),
                                         y - (int)( hotSpot.y() ) ) );
        mark->setLabelRect( labelRect );

        // Add the current placemark to the matching row and its
        // direct neighbors.
        int idx = y / m_maxLabelHeight;
        if ( idx - 1 >= 0 )
            rowsection[ idx - 1 ].append( mark );
        rowsection[ idx ].append( mark );
        if ( idx + 1 < secnumber )
            rowsection[ idx + 1 ].append( mark );

        m_paintOrder.append( mark );
    }

    /**
     * Now handle all other placemarks...
     */
    const QItemSelection selection = m_selectionModel->selection();

    const int rowCount = m_placemarkModel->rowCount();

    for ( int i = 0; i != rowCount; ++i )
    {
        const QModelIndex& index = m_placemarkModel->index( i, 0 );
        if( !index.isValid() ) {
            mDebug() << "invalid index!!!";
            continue;
        }

        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
        GeoDataGeometry *geometry = placemark->geometry();
        if( !dynamic_cast<GeoDataPoint*>(geometry) ) {
            continue;
        }

        int popularityIndex = placemark->popularityIndex();

        
        if ( popularityIndex < 1 ) {
            break;
        }

        // Skip the places that are too small.
        if ( m_weightfilter.at( popularityIndex ) > viewParams->radius() ) {
            break;
        }

        GeoDataCoordinates geopoint = placemark->coordinate();

        if ( !latLonAltBox.contains( geopoint ) ||
             ! viewParams->currentProjection()->screenCoordinates( geopoint, viewParams->viewport(), x, y ))
            {
                delete m_visiblePlacemarks.take( index );
                continue;
            }

        const int visualCategory  = placemark->visualCategory();

        // Skip city marks if we're not showing cities.
        if ( !showCities
             && ( visualCategory > 2 && visualCategory < 20 ) )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( !showTerrain
             && (    visualCategory >= (int)(GeoDataFeature::Mountain) ) 
                  && visualCategory <= (int)(GeoDataFeature::OtherTerrain) )
            continue;

        // Skip other places if we're not showing other places.
        if ( !showOtherPlaces
             && (    visualCategory >= (int)(GeoDataFeature::GeographicPole) ) 
                  && visualCategory <= (int)(GeoDataFeature::Observatory) )
            continue;

        // Skip landing sites if we're not showing landing sites.
        if ( !showLandingSites
             && (    visualCategory >= (int)(GeoDataFeature::MannedLandingSite) ) 
                  && visualCategory <= (int)(GeoDataFeature::UnmannedHardLandingSite) )
            continue;

        // Skip craters if we're not showing craters.
        if ( !showCraters
             && (    visualCategory == (int)(GeoDataFeature::Crater) ) )
            continue;

        // Skip maria if we're not showing maria.
        if ( !showMaria
             && (    visualCategory == (int)(GeoDataFeature::Mare) ) )
            continue;

        const bool isSelected = selection.contains( index );

        /**
         * We handled selected placemarks already, so we skip them here...
         * Assuming that only a small amount of places is selected
         * we check for the selected state after all other filters
         */
        if ( isSelected )
            continue;

        // ----------------------------------------------------------------
        // End of checks. Here the actual layouting starts.
        int textWidth = 0;

        // Find the corresponding visible placemark
        VisiblePlacemark *mark = m_visiblePlacemarks.value( index );
        GeoDataStyle* style = placemark->style();

        // Specify font properties
        if ( mark ) {
            textWidth = mark->labelRect().width();
        }
        else {
            QFont labelFont = style->labelStyle().font();
            textWidth = ( QFontMetrics( labelFont ).width( placemark->name() ) );
        }

        // Choose Section
        const QVector<VisiblePlacemark*> currentsec = rowsection.at( y / m_maxLabelHeight );

         // Find out whether the area around the placemark is covered already.
        // If there's not enough space free don't add a VisiblePlacemark here.

        QRect labelRect = roomForLabel( style, currentsec, x, y, textWidth );
        if ( labelRect.isNull() )
            continue;

        // Make sure not to draw more placemarks on the screen than 
        // specified by placemarksOnScreenLimit().

        ++labelnum;
        if ( labelnum >= placemarksOnScreenLimit() )
            break;

        if ( !mark ) {
            // If there is no visible placemark yet for this index,
            // create a new one...
            mark = new VisiblePlacemark;

            mark->setModelIndex( index );
            m_visiblePlacemarks.insert( index, mark );
        }

        // Finally save the label position on the map.
        QPointF hotSpot = style->iconStyle().hotSpot();

        mark->setSymbolPosition( QPoint( x - (int)( hotSpot.x() ),
                                         y - (int)( hotSpot.y() ) ) );
        mark->setLabelRect( labelRect );

        // Add the current placemark to the matching row and it's
        // direct neighbors.
        int idx = y / m_maxLabelHeight;
        if ( idx - 1 >= 0 )
            rowsection[ idx - 1 ].append( mark );
        rowsection[ idx ].append( mark );
        if ( idx + 1 < secnumber )
            rowsection[ idx + 1 ].append( mark );

        m_paintOrder.append( mark );
    }
    if ( viewParams->mapTheme() )
    {
        QColor labelColor = viewParams->mapTheme()->map()->labelColor();

        m_placemarkPainter->setDefaultLabelColor( labelColor );

    }

    m_placemarkPainter->drawPlacemarks( painter, m_paintOrder, selection, 
                                        viewParams->viewport() );
}

QRect PlacemarkLayout::roomForLabel( GeoDataStyle * style,
                                      const QVector<VisiblePlacemark*> &currentsec,
                                      const int x, const int y,
                                      const int textWidth )
{
    bool  isRoom      = false;

    int symbolwidth = style->iconStyle().icon().width();

    QFont labelFont = style->labelStyle().font();
    int textHeight = QFontMetrics( labelFont ).height();
//    mDebug() << textHeight;

    if ( style->labelStyle().alignment() == GeoDataLabelStyle::Corner ) {
        int  xpos = symbolwidth / 2 + x + 1;
        int  ypos = 0;

        // Check the four possible positions by going through all of them
 
        QRect  labelRect( xpos, ypos, textWidth, textHeight );
    
        while ( xpos >= x - textWidth - symbolwidth - 1 ) {
            ypos = y;

            while ( ypos >= y - textHeight ) {

                isRoom = true;
                labelRect.moveTo( xpos, ypos );

                // Check if there is another label or symbol that overlaps.
                QVector<VisiblePlacemark*>::const_iterator beforeItEnd = currentsec.constEnd();
                for ( QVector<VisiblePlacemark*>::ConstIterator beforeIt = currentsec.constBegin();
                      beforeIt != beforeItEnd;
                      ++beforeIt )
                {
                    if ( labelRect.intersects( (*beforeIt)->labelRect()) ) {
                        isRoom = false;
                        break;
                    }
                }

                if ( isRoom ) {
                    // claim the place immediately if it hasn't been used yet 
                    return labelRect;
                }

                ypos -= textHeight;
            }

            xpos -= ( symbolwidth + textWidth + 2 );
        }
    }
    else if ( style->labelStyle().alignment() == GeoDataLabelStyle::Center ) {
        isRoom = true;
        QRect  labelRect( x - textWidth / 2, y - textHeight / 2, 
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

    return QRect(); // At this point there is no space left 
                    // for the rectangle anymore.
}

int PlacemarkLayout::placemarksOnScreenLimit() const
{
    // For now we just return 100.
    // Later on once we focus on decent high dpi print quality
    // we should replace this static value by a dynamic value
    // that takes the area that gets displayed into account.
    return 100;
}

#include "PlacemarkLayout.moc"
