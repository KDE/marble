//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "PlaceMarkLayout.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QVector>
#include <QtCore/QVectorIterator>
#include <QtGui/QFont>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPainter>

#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"

#include "global.h"
#include "PlaceMarkPainter.h"
#include "MarblePlacemarkModel.h"
#include "MarbleDirs.h"
#include "ViewParams.h"
#include "VisiblePlaceMark.h"

PlaceMarkLayout::PlaceMarkLayout( QObject* parent )
    : QObject( parent )
{
    m_maxLabelHeight = 0;
    m_styleResetRequested = true;

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

    m_placeMarkPainter =  new PlaceMarkPainter( this );
}

PlaceMarkLayout::~PlaceMarkLayout()
{
    styleReset();
}

void PlaceMarkLayout::requestStyleReset()
{
    qDebug() << "Style reset requested.";
    m_styleResetRequested = true;
}

void PlaceMarkLayout::styleReset()
{
    m_paintOrder.clear();
    qDeleteAll( m_visiblePlaceMarks );
    m_visiblePlaceMarks.clear();
}

QVector<QPersistentModelIndex> PlaceMarkLayout::whichPlaceMarkAt( const QPoint& curpos )
{
    if ( m_styleResetRequested == true ) {
        styleReset();
    }

    QVector<QPersistentModelIndex> ret;

    QVector<VisiblePlaceMark*>::const_iterator  it;
    for ( it = m_paintOrder.constBegin();
          it != m_paintOrder.constEnd();
          it++ )
    {
        const VisiblePlaceMark  *mark = *it; // no cast

        if ( mark->labelRect().contains( curpos )
             || QRect( mark->symbolPosition(), mark->symbolPixmap().size() ).contains( curpos ) ) {
            ret.append( mark->modelIndex() );
        }
    }

    return ret;
}

PlaceMarkPainter* PlaceMarkLayout::placeMarkPainter() const
{ 
    return m_placeMarkPainter; 
}

int PlaceMarkLayout::maxLabelHeight( const QAbstractItemModel* model,
                                     const QItemSelectionModel* selectionModel ) const
{
    qDebug() << "Detecting maxLabelHeight ...";

    int maxLabelHeight = 0;

    const QModelIndexList selectedIndexes = selectionModel->selection().indexes();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );
        GeoDataStyle* style = ( ( MarblePlacemarkModel* )index.model() )->styleData( index );
        QFont labelFont = style->labelStyle()->font();
        int textHeight = QFontMetrics( labelFont ).height();
        if ( textHeight > maxLabelHeight )
            maxLabelHeight = textHeight; 
    }

    for ( int i = 0; i < model->rowCount(); ++i )
    {
        QModelIndex index = model->index( i, 0 );

        GeoDataStyle* style = ( ( MarblePlacemarkModel* )index.model() )->styleData( index );
        QFont labelFont = style->labelStyle()->font();
        int textHeight = QFontMetrics( labelFont ).height();
        if ( textHeight > maxLabelHeight ) 
            maxLabelHeight = textHeight; 
    }

    qDebug() <<"Detected maxLabelHeight: " << maxLabelHeight;
    return maxLabelHeight;
}

void PlaceMarkLayout::paintPlaceFolder(QPainter* painter,
                                        ViewParams *viewParams,
                                        const QAbstractItemModel* model,
                                        const QItemSelectionModel* selectionModel,
                                        bool firstTime )
{
    const int imgwidth  = viewParams->m_canvasImage->width();
    const int imgheight = viewParams->m_canvasImage->height();

    if ( m_styleResetRequested == true ) {
        m_styleResetRequested = false;
        styleReset();
        m_persistentIndexList = ( (MarblePlacemarkModel*) model )->persistentIndexList();
        m_maxLabelHeight = maxLabelHeight( model, selectionModel );
    }
    const int   secnumber         = imgheight / m_maxLabelHeight + 1;

//    Quaternion  inversePlanetAxis = viewParams->m_planetAxis.inverse();
    matrix  planetAxisMatrix;
    viewParams->m_planetAxis.inverse().toMatrix( planetAxisMatrix );

    QVector< QVector< VisiblePlaceMark* > >  rowsection;
    for ( int i = 0; i < secnumber; i++)
        rowsection.append( QVector<VisiblePlaceMark*>( ) );

    m_paintOrder.clear();

    int labelnum = 0;
    int x = 0;
    int y = 0;

    /**
     * First handle the selected place marks, as they have the highest priority.
     */

    const QModelIndexList selectedIndexes = selectionModel->selection().indexes();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );

        if ( !locatedOnScreen ( ( ( MarblePlacemarkModel* )index.model() )->coordinateData( index ), x, y, imgwidth, imgheight, planetAxisMatrix, viewParams ) )
        {
            delete m_visiblePlaceMarks.take( index );
            continue;
        }

        // ----------------------------------------------------------------
        // End of checks. Here the actual layouting starts.
        int textWidth = 0;

        // Find the corresponding visible place mark
        VisiblePlaceMark *mark = m_visiblePlaceMarks.value( index );

        GeoDataStyle* style = ( ( MarblePlacemarkModel* )index.model() )->styleData( index );

        // Specify font properties
        if ( mark ) {
            textWidth = mark->labelRect().width();
        }
        else {
            QFont labelFont = style->labelStyle()->font();
            labelFont.setWeight( 75 ); // Needed to calculate the correct pixmap size; 

            textWidth = ( QFontMetrics( labelFont ).width( index.data( Qt::DisplayRole ).toString() )
                  + (int)( 2 * s_labelOutlineWidth ) );
        }

        // Choose Section
        const QVector<VisiblePlaceMark*> currentsec = rowsection.at( y / m_maxLabelHeight );

        // Find out whether the area around the placemark is covered already.
        // If there's not enough space free don't add a VisiblePlaceMark here.

        QRect labelRect = roomForLabel( style, currentsec, x, y, textWidth );
        if ( labelRect.isNull() ) continue;

        // Make sure not to draw more placemarks on the screen than 
        // specified by placeMarksOnScreenLimit().

        ++labelnum;
        if ( labelnum >= placeMarksOnScreenLimit() )
            break;
        if ( !mark )
        {
            // If there is no visible place mark yet for this index, create a new one...
            mark = new VisiblePlaceMark;
            mark->setModelIndex( QPersistentModelIndex( index ) );

            m_visiblePlaceMarks.insert( index, mark );
        }

        // Finally save the label position on the map.
        QPointF hotSpot = style->iconStyle()->hotSpot();

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

    /**
     * Now handle all other place marks...
     */
    const QModelIndex firstIndex = model->index( 0, 0 );
    const int firstPopularity = firstIndex.data( MarblePlacemarkModel::PopularityRole ).toInt();
    const bool  noFilter = ( firstPopularity == 0 
                             || ( firstPopularity != 0
                             && firstIndex.data( MarblePlacemarkModel::GeoTypeRole ).toChar().isNull() ) ) 
                           ? true : false;
    const QItemSelection selection = selectionModel->selection();

    QList<QPersistentModelIndex>::ConstIterator i;
    for ( i = m_persistentIndexList.begin(); i != m_persistentIndexList.end(); ++i )
    {
        const QPersistentModelIndex& index = *i;

        int popularityIndex = index.data( MarblePlacemarkModel::PopularityIndexRole ).toInt();

        // Skip the places that are too small.
        if ( noFilter == false ) {
            if ( m_weightfilter.at( popularityIndex ) > viewParams->m_radius )
            {
                break;
            }
        }

        if ( !locatedOnScreen ( ( ( MarblePlacemarkModel* )index.model() )->coordinateData( index ), x, y, imgwidth, imgheight, planetAxisMatrix, viewParams ) )
        {
            delete m_visiblePlaceMarks.take( index );
            continue;
        }

        const int visualCategory  = index.data( MarblePlacemarkModel::VisualCategoryRole ).toInt();

        // Skip city marks if we're not showing cities.
        if ( !viewParams->m_showCities
             && ( visualCategory > 2 && visualCategory < 20 ) )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( !viewParams->m_showTerrain
             && ( visualCategory >= 20 && visualCategory <= 23 ) )
            continue;

        if ( !viewParams->m_showOtherPlaces
             && ( visualCategory >= 24 && visualCategory <= 27 ) )
            continue;


        const bool isSelected = selection.contains( index );

        /**
         * We handled selected place marks already, so we skip them here...
         * Assuming that only a small amount of places is selected
         * we check for the selected state after all other filters
         */
        if ( isSelected )
            continue;

        // ----------------------------------------------------------------
        // End of checks. Here the actual layouting starts.
        int textWidth = 0;

        // Find the corresponding visible place mark
        VisiblePlaceMark *mark = m_visiblePlaceMarks.value( index );
        GeoDataStyle* style = ( ( MarblePlacemarkModel* )index.model() )->styleData( index );

        // Specify font properties
        if ( mark ) {
            textWidth = mark->labelRect().width();
        }
        else {
            QFont labelFont = style->labelStyle()->font();
            textWidth = ( QFontMetrics( labelFont ).width( index.data( Qt::DisplayRole ).toString() ) );
        }

        // Choose Section
        const QVector<VisiblePlaceMark*> currentsec = rowsection.at( y / m_maxLabelHeight );

         // Find out whether the area around the placemark is covered already.
        // If there's not enough space free don't add a VisiblePlaceMark here.

        QRect labelRect = roomForLabel( style, currentsec, x, y, textWidth );
        if ( labelRect.isNull() ) continue;

        // Make sure not to draw more placemarks on the screen than 
        // specified by placeMarksOnScreenLimit().

        ++labelnum;
        if ( labelnum >= placeMarksOnScreenLimit() )
            break;

        if ( !mark )
        {
            // If there is no visible place mark yet for this index, create a new one...
            mark = new VisiblePlaceMark;

            mark->setModelIndex( index );
            m_visiblePlaceMarks.insert( index, mark );
        }

        // Finally save the label position on the map.
        QPointF hotSpot = style->iconStyle()->hotSpot();

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
    m_placeMarkPainter->drawPlaceMarks( painter, m_paintOrder, selection, viewParams );
}

inline bool PlaceMarkLayout::locatedOnScreen ( const GeoDataPoint &geopoint, 
                                               int &x, int &y, 
                                               const int &imgwidth, const int &imgheight,
                                               const matrix &planetAxisMatrix,
                                               ViewParams * viewParams )
{
    if( viewParams->m_projection == Spherical ) {
    
        double absoluteAltitude = geopoint.altitude() + EARTH_RADIUS;
        Quaternion qpos = ( geopoint ).quaternion();
        //    Quaternion qpos = ( index.data().value<GeoDataPoint>() ).quaternion();
        qpos.rotateAroundAxis( planetAxisMatrix );

        double pixelAltitude = ( viewParams->m_radius )/ EARTH_RADIUS * absoluteAltitude;
	    if (geopoint.altitude() < 10000) {
	        // Skip placemarks at the other side of the earth.
	        if ( qpos.v[Q_Z] < 0 ) {
	            return false;
	        }
	    }
	    else {
            double earthCenteredX=pixelAltitude * qpos.v[Q_X];
            double earthCenteredY=pixelAltitude * qpos.v[Q_Y];
            // don't draw high placemarks (e.g. satellites) that are not visible
	        if ( qpos.v[Q_Z] < 0 && (earthCenteredX*earthCenteredX + earthCenteredY*earthCenteredY) < viewParams->m_radius * viewParams->m_radius)
	            return false;
	    }
        // Let (x, y) be the position on the screen of the placemark..
        x = (int)(imgwidth  / 2 + pixelAltitude * qpos.v[Q_X]);
        y = (int)(imgheight / 2 - pixelAltitude * qpos.v[Q_Y]);

        // Skip placemarks that are outside the screen area
        if ( x < 0 || x >= imgwidth || y < 0 || y >= imgheight ) {
            return false;
        }

        return true;
    }

    if( viewParams->m_projection == Equirectangular ) {

            double lon, lat;
            double rad2Pixel = 2 * viewParams->m_radius / M_PI;

            double centerLon, centerLat;
            viewParams->centerCoordinates( centerLon, centerLat );

            geopoint.geoCoordinates( lon, lat );

            // Let (x, y) be the position on the screen of the placemark..
            x = (int)(imgwidth  / 2 - rad2Pixel * (centerLon - lon));
            y = (int)(imgheight / 2 + rad2Pixel * (centerLat - lat));

            // Skip placemarks that are outside the screen area
            //
            if ( (y >= 0 && y < imgheight)
                 && ( (x >= 0 && x < imgwidth) 
                   || (x - 4 * viewParams->m_radius >= 0 && x - 4 * viewParams->m_radius < imgwidth)
                   || (x + 4 * viewParams->m_radius >= 0 && x + 4 * viewParams->m_radius < imgwidth) ) 
               )
            {
                return true;
            }

            return false;
    }

    return true;
}

QRect PlaceMarkLayout::roomForLabel( GeoDataStyle * style,
                                      const QVector<VisiblePlaceMark*> &currentsec,
                                      const int x, const int y,
                                      const int textWidth )
{
    bool  isRoom      = false;

    int symbolwidth = style->iconStyle()->icon().width();

    QFont labelFont = style->labelStyle()->font();
    int textHeight = QFontMetrics( labelFont ).height();
//    qDebug() << textHeight;

    if( style->labelStyle()->alignment() == GeoDataLabelStyle::Corner )
    {
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
                for ( QVector<VisiblePlaceMark*>::const_iterator beforeit = currentsec.constBegin();
                      beforeit != currentsec.constEnd();
                      ++beforeit )
                {
                    if ( labelRect.intersects( (*beforeit)->labelRect()) ) {
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
    else if( style->labelStyle()->alignment() == GeoDataLabelStyle::Center )
    {
        isRoom = true;
        QRect  labelRect( x - textWidth / 2, y - textHeight / 2, textWidth, textHeight );

        // Check if there is another label or symbol that overlaps.
        for ( QVector<VisiblePlaceMark*>::const_iterator beforeit = currentsec.constBegin();
              beforeit != currentsec.constEnd();
              ++beforeit )
        {
            if ( labelRect.intersects( (*beforeit)->labelRect()) ) {
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

int PlaceMarkLayout::placeMarksOnScreenLimit() const
{
    // For now we just return 100.
    // Later on once we focus on decent high dpi print quality
    // we should replace this static value by a dynamic value
    // that takes the area that gets displayed into account.
    return 100;
}


#include "PlaceMarkLayout.moc"
