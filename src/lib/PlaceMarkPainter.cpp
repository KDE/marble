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

#include "PlaceMarkPainter.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QVector>
#include <QtCore/QVectorIterator>
#include <QtGui/QIcon>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPainter>

#include "global.h"
#include "PlaceMark.h"
#include "PlaceMarkModel.h"
#include "MarbleDirs.h"
#include "ViewParams.h"


// ================================================================
//                     class VisiblePlaceMark


class VisiblePlaceMark
{
 public:
    VisiblePlaceMark();
    ~VisiblePlaceMark();

    const QPersistentModelIndex& modelIndex() const { return m_modelIndex; }
    void setModelIndex( const QPersistentModelIndex &modelIndex ) { m_modelIndex = modelIndex; }

    const QString name() const
    {
        if ( m_name.isEmpty() )
            m_name = m_modelIndex.data( Qt::DisplayRole ).toString();

        return m_name;
    }
    const QPixmap& symbolPixmap() const
    {
        if ( m_symbolPixmap.isNull() )
            m_symbolPixmap = m_modelIndex.data( Qt::DecorationRole ).value<QPixmap>();
        return  m_symbolPixmap;
    }
    const QSize& symbolSize() const
    {
//        FIXME: tokoe: In some cases we do get an uninitialized m_symbolSize. Why? 
//        if ( m_symbolSize.isNull() )
//        {
            m_symbolSize = m_modelIndex.data( PlaceMarkModel::SymbolSizeRole ).toSize();
            return m_symbolSize;
//        }

    }
    const int symbolIndex() const
    {
        if ( m_symbolIndex == 0 )
            m_symbolIndex = m_modelIndex.data( PlaceMarkModel::SymbolIndexRole ).toInt();
        return m_symbolIndex;
    }
    const QChar role() const { return m_modelIndex.data( PlaceMarkModel::GeoTypeRole ).toChar(); }

    const QPoint& symbolPos() const { return m_symbolPos; }
    void setSymbolPos( const QPoint& sympos ) { m_symbolPos = sympos; }
    void setSymbolPos( const int x, const int y ) { m_symbolPos = QPoint( x, y ); }

    const QPixmap& labelPixmap() const { return m_labelPixmap; }
    void setLabelPixmap( const QPixmap& labelPixmap ) { m_labelPixmap = labelPixmap; }

    const QRect& labelRect() const { return m_labelRect; }
    void setLabelRect( const QRect& labelRect ) { m_labelRect = labelRect; }

 private:
    QPersistentModelIndex m_modelIndex;

    // View stuff
    QPoint      m_symbolPos;	// position of the placemark's symbol
    QPixmap     m_labelPixmap;	// the text label (most often name)
    QRect       m_labelRect;    // bounding box of label

    mutable QString     m_name;         // cached value
    mutable QSize       m_symbolSize;   // cached value
    mutable int         m_symbolIndex;  // cached value
    mutable QPixmap     m_symbolPixmap; // cached value
};

VisiblePlaceMark::VisiblePlaceMark()
  : m_symbolIndex( 0 ), 
    m_symbolSize( QSize( 0, 0 ) )
{
}


VisiblePlaceMark::~VisiblePlaceMark()
{
}

// ================================================================
//                     class PlaceMarkPainter


static const int  maxlabels = 100;
static const double s_labelOutlineWidth = 2.5;

PlaceMarkPainter::PlaceMarkPainter( QObject* parent )
    : QObject( parent )
{
#ifdef Q_OS_MACX
    m_font_mountain = QFont( "Sans Serif",9, 50, false );

    m_font_regular           = QFont( "Sans Serif", 10, 50, false );
    m_font_regular_italics   = QFont( "Sans Serif", 10, 50, true );
    m_font_regular_underline = QFont( "Sans Serif", 10, 50, false );
#else
    m_font_mountain          = QFont( "Sans Serif",  7, 50, false );

    m_font_regular           = QFont( "Sans Serif",  8, 50, false );
    m_font_regular_italics   = QFont( "Sans Serif",  8, 50, true );
    m_font_regular_underline = QFont( "Sans Serif",  8, 50, false );
#endif

    m_font_regular_underline.setUnderline( true );

    m_fontheight = QFontMetrics( m_font_regular ).height();
    m_fontascent = QFontMetrics( m_font_regular ).ascent();

    m_labelareaheight = 2 * m_fontheight;

    m_labelcolor = QColor( 0, 0, 0, 255 );

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

    m_useXWorkaround = testXBug();
    qDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );
}

PlaceMarkPainter::~PlaceMarkPainter()
{
    m_visiblePlaceMarks.clear();

    qDeleteAll( m_allPlaceMarks );
    m_allPlaceMarks.clear();
}

void PlaceMarkPainter::setLabelColor( const QColor &color )
{
    m_labelcolor = color;

    QHash<QPersistentModelIndex, VisiblePlaceMark*>::const_iterator i = m_allPlaceMarks.constBegin();
    while (i != m_allPlaceMarks.constEnd())
    {
        i.value()->setLabelPixmap( QPixmap() );
        ++i;
    }
}

QVector<QPersistentModelIndex> PlaceMarkPainter::whichPlaceMarkAt( const QPoint& curpos ) const
{
    QVector<QPersistentModelIndex> ret;

    QVector<VisiblePlaceMark*>::const_iterator  it;
    for ( it = m_visiblePlaceMarks.constBegin();
          it != m_visiblePlaceMarks.constEnd();
          it++ )
    {
        const VisiblePlaceMark  *mark = *it; // no cast

        if ( mark->labelRect().contains( curpos )
             || QRect( mark->symbolPos(), mark->symbolSize() ).contains( curpos ) ) {
            ret.append( mark->modelIndex() );
        }
    }

    return ret;
}


void PlaceMarkPainter::paintPlaceFolder(QPainter* painter,
                                        int imgwidth, int imgheight,
                                        ViewParams *viewParams,
                                        const QAbstractItemModel* model,
                                        const QItemSelectionModel* selectionModel,
                                        Quaternion planetAxis,
                                        bool firstTime )
{
    switch( viewParams->m_projection ) {
        case Spherical:
            sphericalPaintPlaceFolder(    painter, imgwidth, imgheight,
                                          viewParams, model, selectionModel,
                                          planetAxis, firstTime );
            break;
        case Equirectangular:
            rectangularPaintPlaceFolder(  painter, imgwidth, imgheight,
                                          viewParams, model, selectionModel,
                                          planetAxis, firstTime );
            break;
        default:
            qDebug()<<"Projection not supported";
    }

}

void PlaceMarkPainter::sphericalPaintPlaceFolder(QPainter* painter,
                                        int imgwidth, int imgheight,
                                        ViewParams *viewParams,
                                        const QAbstractItemModel* model,
                                        const QItemSelectionModel* selectionModel,
                                        Quaternion planetAxis,
                                        bool firstTime )
{
    int x = 0;
    int y = 0;

    int secnumber = imgheight / m_labelareaheight + 1;

    Quaternion invplanetAxis = planetAxis.inverse();
    Quaternion qpos;

    painter->setPen( QColor( Qt::black ) );

    QVector< QVector< VisiblePlaceMark* > >  rowsection;
    for ( int i = 0; i < secnumber; i++)
        rowsection.append( QVector<VisiblePlaceMark*>( ) );

    m_visiblePlaceMarks.clear();

    int labelnum = 0;

    /**
     * First handle the selected place marks, as they have the highest priority.
     */

    const QModelIndexList selectedIndexes = selectionModel->selection().indexes();

    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );

        qpos = index.data( PlaceMarkModel::CoordinateRole ).value<GeoPoint>().quaternion();
        qpos.rotateAroundAxis( invplanetAxis );

        // Let (x, y) be the position on the screen of the placemark..
        x = (int)(imgwidth  / 2 + viewParams->m_radius * qpos.v[Q_X]);
        y = (int)(imgheight / 2 + viewParams->m_radius * qpos.v[Q_Y]);

        // Skip placemarks that are outside the screen area
        if ( x < 0 || x >= imgwidth || y < 0 || y >= imgheight ) {
            delete m_allPlaceMarks.take( index );
            continue;
        }

        QFont font;
        int textWidth = 0;

        // Find the corresponding visible place mark
        VisiblePlaceMark *mark = m_allPlaceMarks.value( index );

        // If there is no visible place mark yet for this index, create a new one...
        if ( !mark ) {
            mark = new VisiblePlaceMark;
            mark->setModelIndex( index );
            m_allPlaceMarks.insert( index, mark );
        }

        const QPixmap labelPixmap = mark->labelPixmap();

        // Choose Section
        const QVector<VisiblePlaceMark*> currentsec = rowsection.at( y / m_labelareaheight );

        // Specify font properties
        if ( labelPixmap.isNull() ) {
            labelFontData( mark, font, textWidth, true );
        }
        else {
            textWidth = ( mark->labelRect() ).width();
        }

        // Find out whether the area around the placemark is covered already.
        if ( !roomForLabel( currentsec, mark,
                            textWidth, x, y ) )
        {
            // Don't clear the label pixmap here, for the label should
            // really be painted, but there is just too many other
            // labels around it so there is no room.
            continue;
        }

        // Paint the label
        if ( labelPixmap.isNull() )
            drawLabelPixmap( mark, textWidth, font, true );

        // Finally save the label position on the map.
        const QSize symbolSize = mark->symbolSize();
        mark->setSymbolPos( QPoint( x - symbolSize.width()  / 2,
                                    y - symbolSize.height() / 2) );

        // Add the current placemark to the matching row and it's
        // direct neighbors.
        int idx = y / m_labelareaheight;
        if ( idx - 1 >= 0 )
            rowsection[ idx - 1 ].append( mark );
        rowsection[ idx ].append( mark );
        if ( idx + 1 < secnumber )
            rowsection[ idx + 1 ].append( mark );

        m_visiblePlaceMarks.append( mark );

        labelnum ++;
        if ( labelnum >= maxlabels )
            break;
    }

    /**
     * Now handle all other place marks...
     */
    const QModelIndex firstIndex = model->index( 0, 0 );
    const int firstPopulation = firstIndex.data( PlaceMarkModel::PopulationRole ).toInt();
    const bool  noFilter = ( firstPopulation == 0 
                             || ( firstPopulation != 0
                             && firstIndex.data( PlaceMarkModel::GeoTypeRole ).toChar().isNull() ) ) 
                           ? true : false;
    for ( int i = 0; i < model->rowCount(); ++i )
    {
        const QModelIndex index = model->index( i, 0 );

        const bool isSelected = selectionModel->selection().contains( index );

        /**
         * We handled selected place marks already, so skip here...
         */
        if ( isSelected )
            continue;

        const int populationIndex = index.data( PlaceMarkModel::PopulationIndexRole ).toInt();

        // Skip the places that are too small.
        if ( noFilter == false ) {
            if ( m_weightfilter.at( populationIndex ) > viewParams->m_radius )
                continue;
        }

        const int symbolIndex = index.data( PlaceMarkModel::SymbolIndexRole ).toInt();

        // Skip city marks if we're not showing cities.
        if ( !viewParams->m_showCities
             && ( symbolIndex >= 0 && symbolIndex < 16 ) )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( !viewParams->m_showTerrain
             && ( symbolIndex >= 16 && symbolIndex <= 20 ) )
            continue;

        qpos = index.data( PlaceMarkModel::CoordinateRole ).value<GeoPoint>().quaternion();
        qpos.rotateAroundAxis( invplanetAxis );

        // Skip placemarks at the other side of the earth.
        if ( qpos.v[Q_Z] < 0 ) {
            delete m_allPlaceMarks.take( index );
            continue;
        }

        // Let (x, y) be the position on the screen of the placemark..
        x = (int)(imgwidth  / 2 + viewParams->m_radius * qpos.v[Q_X]);
        y = (int)(imgheight / 2 + viewParams->m_radius * qpos.v[Q_Y]);

        // Skip placemarks that are outside the screen area
        if ( x < 0 || x >= imgwidth || y < 0 || y >= imgheight ) {
            delete m_allPlaceMarks.take( index );
            continue;
        }

        // ----------------------------------------------------------------
        // End of checks. Here the actual painting starts.

        QFont font;
        int textWidth = 0;

        // Find the corresponding visible place mark
        VisiblePlaceMark *mark = m_allPlaceMarks.value( index );

        // If there is no visible place mark yet for this index, create a new one...
        if ( !mark ) {
            mark = new VisiblePlaceMark;
            mark->setModelIndex( index );
            m_allPlaceMarks.insert( index, mark );
        }

        const QPixmap labelPixmap = mark->labelPixmap();

        // Choose Section
        const QVector<VisiblePlaceMark*> currentsec = rowsection.at( y / m_labelareaheight );

        // Specify font properties
        if ( labelPixmap.isNull() ) {
            labelFontData( mark, font, textWidth, isSelected );
        }
        else {
            textWidth = ( mark->labelRect() ).width();
        }

        // Find out whether the area around the placemark is covered already.
        if ( !roomForLabel( currentsec, mark,
                            textWidth, x, y ) )
        {
            // Don't clear the label pixmap here, for the label should
            // really be painted, but there is just too many other
            // labels around it so there is no room.
            continue;
        }

        // Paint the label
        if ( labelPixmap.isNull() )
        {
            drawLabelPixmap( mark, textWidth, font, isSelected );
        }

        // Finally save the label position on the map.
        // FIXME: This assumes that the hotspot is the center of the symbol.
        const QSize symbolSize = mark->symbolSize();
        mark->setSymbolPos( QPoint( x - symbolSize.width()  / 2,
                                    y - symbolSize.height() / 2) );

        // Add the current placemark to the matching row and it's
        // direct neighbors.
        int idx = y / m_labelareaheight;
        if ( idx - 1 >= 0 )
            rowsection[ idx - 1 ].append( mark );
        rowsection[ idx ].append( mark );
        if ( idx + 1 < secnumber )
            rowsection[ idx + 1 ].append( mark );

        m_visiblePlaceMarks.append( mark );

        labelnum ++;
        if ( labelnum >= maxlabels )
            break;
    }

    QVector<VisiblePlaceMark*>::const_iterator visit = m_visiblePlaceMarks.constEnd();

    VisiblePlaceMark *mark = 0;
    while ( visit != m_visiblePlaceMarks.constBegin() ) {
        --visit;
        mark = *visit;

        painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
        painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
    }
}


void PlaceMarkPainter::rectangularPaintPlaceFolder(QPainter* painter,
                                        int imgwidth, int imgheight,
                                        ViewParams *viewParams,
                                        const QAbstractItemModel* model,
                                        const QItemSelectionModel* selectionModel,
                                        Quaternion planetAxis,
                                        bool firstTime )
{
    int  x = 0;
    int  y = 0;

    int  secnumber = imgheight / m_labelareaheight + 1;

    Quaternion qpos;

    painter->setPen( QColor( Qt::black ) );

    // Calculate translation of center point
    double centerLat = planetAxis.pitch() + M_PI;
    if ( centerLat > M_PI ) centerLat -= 2 * M_PI; 
    double centerLon = planetAxis.yaw() + M_PI;

    double const xyFactor = 2 * viewParams->m_radius / M_PI;
    double degX;
    double degY;

    QVector< QVector< VisiblePlaceMark* > > rowsection;
    for ( int i = 0; i < secnumber; i++)
        rowsection.append( QVector<VisiblePlaceMark*>() );

    m_visiblePlaceMarks.clear();

    int labelnum = 0;

    /**
     * First handle the selected place marks, as they have the highest priority.
     */
    const QModelIndexList selectedIndexes = selectionModel->selection().indexes();
    for ( int i = 0; i < selectedIndexes.count(); ++i ) {
        const QModelIndex index = selectedIndexes.at( i );

        qpos = index.data( PlaceMarkModel::CoordinateRole ).value<GeoPoint>().quaternion();

        // Let (x, y) be the position on the screen of the placemark..
        qpos.getSpherical( degX, degY );
        x = (int)(imgwidth  / 2 + xyFactor * (degX + centerLon));
        y = (int)(imgheight / 2 + xyFactor * (degY + centerLat));

        QFont font;
        int textWidth = 0;

        // Find the corresponding visible place mark
        VisiblePlaceMark *mark = m_allPlaceMarks.value( index );

        // If there is no visible place mark yet for this index, create a new one...
        if ( !mark ) {
            mark = new VisiblePlaceMark;
            mark->setModelIndex( QPersistentModelIndex( index ) );
            m_allPlaceMarks.insert( index, mark );
        }

        // Choose Section
        const QVector<VisiblePlaceMark*> currentsec = rowsection.at( y / m_labelareaheight );

        const QPixmap labelPixmap = mark->labelPixmap();

        // Specify font properties
        if ( labelPixmap.isNull() ) {
            labelFontData( mark, font, textWidth, true );
        } else {
            textWidth = ( mark->labelRect() ).width();
        }

        // Find out whether the area around the placemark is covered already.
        if ( !roomForLabel( currentsec, mark,
                            textWidth, x, y ) )
        {
            // Don't clear the text pixmap here, for the label should
            // really be painted, but there is just too many other
            // labels around it so there is no room.
            continue;
        }

        // Paint the label
        if ( labelPixmap.isNull() ) {
            drawLabelPixmap( mark, textWidth, font, true );
        }

        // Finally save the label position on the map.
        const QSize symbolSize = mark->symbolSize();
        mark->setSymbolPos( QPoint( x - symbolSize.width()  / 2,
                                    y - symbolSize.height() / 2) );

        // Add the current placemark to the matching row and it's
        // direct neighbors.
        int idx = y / m_labelareaheight;
        if ( idx - 1 >= 0 )
            rowsection[ idx - 1 ].append( mark );
        rowsection[ idx ].append( mark );
        if ( idx + 1 < secnumber )
            rowsection[ idx + 1 ].append( mark );

        m_visiblePlaceMarks.append( mark );

        labelnum ++;
        if ( labelnum >= maxlabels )
            break;
    }

    const QModelIndex firstIndex = model->index( 0, 0 );
    const bool  noFilter = ( firstIndex.data( PlaceMarkModel::PopulationRole ).toInt() == 0 
                             || ( firstIndex.data( PlaceMarkModel::PopulationRole ).toInt() != 0
                             && firstIndex.data( PlaceMarkModel::GeoTypeRole ).toChar().isNull() ) ) 
                           ? true : false;

    /**
     * Now handle all other place marks...
     */
    for ( int i = 0; i < model->rowCount(); ++i )
    {
        const QModelIndex index = model->index( i, 0 );

        const bool isSelected = selectionModel->selection().contains( index );

        /**
         * We handled selected place marks already, so skip here...
         */
        if ( isSelected )
            continue;

        const int populationIndex = index.data( PlaceMarkModel::PopulationIndexRole ).toInt();

#ifndef KML_GSOC
        if ( noFilter == false ) {
            if ( m_weightfilter.at( populationIndex ) > viewParams->m_radius
                 && !isSelected )
                continue;
        }
#endif
        const int symbolIndex = index.data( PlaceMarkModel::SymbolIndexRole ).toInt();

        // Skip city marks if we're not showing cities.
        if ( !viewParams->m_showCities
             && ( 0 <= symbolIndex && symbolIndex < 16 ) )
            continue;

        // Skip terrain marks if we're not showing terrain.
        if ( !viewParams->m_showTerrain
             && ( symbolIndex >= 16 && symbolIndex <= 20 ) )
            continue;

        qpos = index.data( PlaceMarkModel::CoordinateRole ).value<GeoPoint>().quaternion();

        // Let (x, y) be the position on the screen of the placemark..
        qpos.getSpherical( degX, degY );
        x = (int)(imgwidth  / 2 + xyFactor * (degX + centerLon));
        y = (int)(imgheight / 2 + xyFactor * (degY + centerLat));

        // Skip placemarks that are outside the screen area
        //
        // FIXME: I have the feeling that this is wrong, because there
        //        are always insanely few placemarks on the flat map 
        //        compared to the globe.
        if ( ( ( x < 0 || x >= imgwidth )
               // FIXME: Carlos: check this:
               && x - 4 * viewParams->m_radius < 0
               && x + 4 * viewParams->m_radius >= imgwidth )
             || y < 0 || y >= imgheight )
        {
            delete m_allPlaceMarks.take( index );
            continue;
        }

        // ----------------------------------------------------------------
        // End of checks. Here the actual painting starts.

        QFont font;
        int textWidth = 0;

        // Find the corresponding visible place mark
        VisiblePlaceMark *mark = m_allPlaceMarks.value( index );

        // If there is no visible place mark yet for this index, create a new one...
        if ( !mark ) {
            mark = new VisiblePlaceMark;
            mark->setModelIndex( QPersistentModelIndex( index ) );
            m_allPlaceMarks.insert( index, mark );
        }

        // Choose Section
        const QVector<VisiblePlaceMark*> currentsec = rowsection.at( y / m_labelareaheight );

        QPixmap labelPixmap = mark->labelPixmap();

        // Specify font properties
        if ( labelPixmap.isNull() ) {
            labelFontData( mark, font, textWidth, isSelected );
        }
        else {
            textWidth = ( mark->labelRect() ).width();
        }

        // Find out whether the area around the placemark is covered already.
        if ( !roomForLabel( currentsec, mark,
                            textWidth, x, y ) )
        {
            // Don't clear the text pixmap here, for the label should
            // really be painted, but there is just too many other
            // labels around it so there is no room.
            continue;
        }

        // Paint the label
        if ( labelPixmap.isNull() ) {
            drawLabelPixmap( mark, textWidth, font, isSelected );
        }

        // Finally save the label position on the map.
        const QSize symbolSize = mark->symbolSize();
        mark->setSymbolPos( QPoint( x - symbolSize.width()  / 2,
                                    y - symbolSize.height() / 2) );

        // Add the current placemark to the matching row and it's
        // direct neighbors.
        int idx = y / m_labelareaheight;
        if ( idx - 1 >= 0 )
            rowsection[ idx - 1 ].append( mark );
        rowsection[ idx ].append( mark );
        if ( idx + 1 < secnumber )
            rowsection[ idx + 1 ].append( mark );

        m_visiblePlaceMarks.append( mark );

        labelnum ++;
        if ( labelnum >= maxlabels )
            break;
    }

    // qDebug() << QString("Size: %1, Rows: %2").arg(m_visiblePlacemarks.size()).arg( secnumber );
    QVector<VisiblePlaceMark*>::const_iterator visit = m_visiblePlaceMarks.constEnd();

    while ( visit != m_visiblePlaceMarks.constBegin() ) {
        --visit;
        VisiblePlaceMark *mark = *visit;

        painter->drawPixmap( mark->labelRect(),  mark->labelPixmap() );
        painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );

        int tempSymbol = mark->symbolPos().x();
        int tempText = mark->labelRect().x();
        for ( int i = tempSymbol - 4 * viewParams->m_radius;
              i >= 0;
              i -= 4 * viewParams->m_radius )
        {
            QRect labelRect( mark->labelRect() );
            labelRect.moveLeft(i - tempSymbol + tempText );
            mark->setLabelRect( labelRect );

            QPoint symbolPos( mark->symbolPos() );
            symbolPos.setX( i );
            mark->setSymbolPos( symbolPos );

            painter->drawPixmap( mark->labelRect(),  mark->labelPixmap() );
            painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
        }

        for ( int i = tempSymbol;
              i <= imgwidth;
              i += 4 * viewParams->m_radius )
        {
            QRect labelRect( mark->labelRect() );
            labelRect.moveLeft(i - tempSymbol + tempText );
            mark->setLabelRect( labelRect );

            QPoint symbolPos( mark->symbolPos() );
            symbolPos.setX( i );
            mark->setSymbolPos( symbolPos );

            painter->drawPixmap( mark->labelRect(),  mark->labelPixmap() );
            painter->drawPixmap( mark->symbolPos(), mark->symbolPixmap() );
        }
    }
}

// Set font and textWidth according to the type of the PlaceMark.
//
void PlaceMarkPainter::labelFontData( VisiblePlaceMark *mark,
                                      QFont &font, int &textWidth, bool isSelected )
{
    const QChar role = mark->role();

    // C: Admin. center of _C_ountry
    // R: Admin. center of _R_egion
    // B: Admin. center of country and region ("_B_oth")
    // N: _N_one
    font = m_font_regular;

    if ( role == 'N' ) {
        font = m_font_regular;
    } else if ( role == 'R' ) {
        font = m_font_regular_italics;
    } else if ( role == 'B' || role == 'C' ) {
        font = m_font_regular_underline;
    } else {
        font = m_font_regular;
    }

    if ( ( mark->symbolIndex() > 13 && mark->symbolIndex() < 16 ) || isSelected )
        font.setWeight( 75 );

    if ( role == 'P' )
        font = m_font_regular;
    if ( role == 'M' )
        font = m_font_regular;
    if ( role == 'H' )
        font = m_font_mountain;
    if ( role == 'V' )
        font = m_font_mountain;

    textWidth = ( QFontMetrics( font ).width( mark->name() )
                  + (int)( s_labelOutlineWidth ) );
}


bool PlaceMarkPainter::roomForLabel( const QVector<VisiblePlaceMark*> &currentsec,
                                     VisiblePlaceMark *mark,
                                     int textwidth,
                                     int x, int y )
{
    bool  isRoom      = false;
    int   symbolwidth = mark->symbolSize().width();

    int  xpos = symbolwidth / 2 + x + 1;
    int  ypos = 0;

    // Check the four possible positions by going through all of them
 
    while ( xpos >= x - textwidth - symbolwidth - 1 ) {
        ypos = y;

        while ( ypos >= y - m_fontheight ) {

            isRoom = true;

            QRect  labelRect( xpos, ypos, textwidth, m_fontheight );

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
                mark->setLabelRect( labelRect );
                return true;
            }

            ypos -= m_fontheight;
        }

        xpos -= ( symbolwidth + textwidth + 2 );
    }

    return isRoom;
}

inline void PlaceMarkPainter::drawLabelText(QPainter& textpainter,
                                            const QString &name, const QFont &font )
{
    QPen    outlinepen( Qt::white );
    outlinepen.setWidthF( s_labelOutlineWidth );
    QBrush  outlinebrush( Qt::black );

    QPainterPath   outlinepath;
    const QPointF  baseline( s_labelOutlineWidth / 2.0, m_fontascent );
    outlinepath.addText( baseline, font, name );
    textpainter.setRenderHint( QPainter::Antialiasing, true );
    textpainter.setPen( outlinepen );
    textpainter.setBrush( outlinebrush );
    textpainter.drawPath( outlinepath );
    textpainter.setPen( Qt::NoPen );
    textpainter.drawPath( outlinepath );
    textpainter.setRenderHint( QPainter::Antialiasing, false );
}

inline void PlaceMarkPainter::drawLabelPixmap( VisiblePlaceMark *mark, int textWidth,
                                               const QFont &font, bool isSelected )
{
    QPainter painter;
    QPixmap labelPixmap;

    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases.
    if ( !m_useXWorkaround ) {
        labelPixmap = QPixmap( textWidth, m_fontheight );
        labelPixmap.fill( Qt::transparent );

        painter.begin( &labelPixmap );

        if ( !isSelected ) {
            painter.setFont( font );
            painter.setPen( m_labelcolor );
            painter.drawText( 0, m_fontascent, mark->name() );
        }
        else {
            drawLabelText( painter, mark->name(), font );
        }

        painter.end();
    } else {
        QImage image( textWidth, m_fontheight,
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        painter.begin( &image );

        if ( !isSelected ) {
            painter.setFont( font );
            painter.setPen( m_labelcolor );
            painter.drawText( 0, m_fontascent, mark->name() );
        }
        else {
            drawLabelText( painter, mark->name(), font );
        }

        painter.end();

        labelPixmap = QPixmap::fromImage( image );
    }

    mark->setLabelPixmap( labelPixmap );
}

// Test if there a bug in the X server which makes 
// text fully transparent if it gets written on 
// QPixmaps that were initialized by filling them 
// with Qt::transparent

bool PlaceMarkPainter::testXBug()
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

    for ( int x = 0; x < fontwidth; x++ ) {
        for ( int y = 0; y < fontheight; y++ ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }
    }

    return true;
}


#include "PlaceMarkPainter.moc"
