//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesModel.h"

#include "EclipsesItem.h"
#include "MarbleDebug.h"
#include "MarbleClock.h"

#include <eclsolar.h>

#include <QIcon>

namespace Marble
{

EclipsesModel::EclipsesModel( const MarbleModel *model, QObject *parent )
    : QAbstractItemModel( parent ),
      m_marbleModel( model ),
      m_currentYear( 0 ),
      m_withLunarEclipses( false )
{
    m_ecl = new EclSolar();
    m_ecl->setTimezone( model->clock()->timezone() / 3600. );
    m_ecl->setLunarEcl( m_withLunarEclipses );

    // oberservation point defaults to home location
    qreal lon, lat;
    int zoom;
    m_marbleModel->home( lon, lat, zoom );
    GeoDataCoordinates homeLocation( lon, lat, 0, GeoDataCoordinates::Degree );
    setObservationPoint( homeLocation );
}

EclipsesModel::~EclipsesModel()
{
    clear();
    delete m_ecl;
}
const GeoDataCoordinates& EclipsesModel::observationPoint() const
{
    return m_observationPoint;
}

void EclipsesModel::setObservationPoint( const GeoDataCoordinates &coords )
{
    m_observationPoint = coords;
    m_ecl->setLocalPos( coords.latitude(), coords.altitude(), 6000. );
}

void EclipsesModel::setYear( int year )
{
    if( m_currentYear != year ) {

        mDebug() << "Year changed - Calculating eclipses...";
        m_currentYear = year;
        m_ecl->putYear( year );

        update();
    }
}

int EclipsesModel::year() const
{
    return m_currentYear;
}

void EclipsesModel::setWithLunarEclipses( const bool enable )
{
    if( m_withLunarEclipses != enable ) {
        m_withLunarEclipses = enable;
        m_ecl->setLunarEcl( m_withLunarEclipses );
        update();
    }
}

bool EclipsesModel::withLunarEclipses() const
{
    return m_withLunarEclipses;
}

EclipsesItem* EclipsesModel::eclipseWithIndex( int index )
{
    foreach( EclipsesItem *item, m_items ) {
        if( item->index() == index ) {
            return item;
        }
    }

    return NULL;
}

QList<EclipsesItem*> EclipsesModel::items() const
{
    return m_items;
}

QModelIndex EclipsesModel::index( int row, int column, const QModelIndex &parent ) const
{
    if( !hasIndex( row, column, parent ) ) {
        return QModelIndex();
    }

    if( row >= m_items.count() ) {
        return QModelIndex();
    }

    return createIndex( row, column, m_items.at( row ) );
}

QModelIndex EclipsesModel::parent( const QModelIndex &index ) const
{
    Q_UNUSED( index );
    return QModelIndex(); // no parents
}

int EclipsesModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return m_items.count();
}

int EclipsesModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 4; // start, end, type magnitude
}

QVariant EclipsesModel::data( const QModelIndex &index, int role ) const
{
    if( !index.isValid() ) {
        return QVariant();
    }

    Q_ASSERT( index.column() < 4 );

    EclipsesItem *item = static_cast<EclipsesItem*>( index.internalPointer() );
    if( role == Qt::DisplayRole ) {
        switch( index.column() ) {
            case 0: return QVariant( item->startDatePartial() );
            case 1: return QVariant( item->endDatePartial() );
            case 2: return QVariant( item->phaseText() );
            case 3: return QVariant( item->magnitude() );
            default: break; // should never happen
        }
    }
    if( role == Qt::DecorationRole ) {
        if ( index.column() == 2 ) return QVariant( item->icon() );
    }

    return QVariant();
}

QVariant EclipsesModel::headerData( int section, Qt::Orientation orientation,
                                    int role ) const
{
    if( orientation != Qt::Horizontal || role != Qt::DisplayRole ) {
        return QVariant();
    }

    switch( section ) {
        case 0: return QVariant( tr( "Start" ) );
        case 1: return QVariant( tr( "End" ) );
        case 2: return QVariant( tr( "Type" ) );
        case 3: return QVariant( tr( "Magnitude" ) );
        default: break;
    }

    return QVariant();
}

void EclipsesModel::addItem( EclipsesItem *item )
{
    m_items.append( item );
}

void EclipsesModel::clear()
{
    beginResetModel();

    qDeleteAll( m_items );
    m_items.clear();

    endResetModel();
}

void EclipsesModel::update()
{
    clear();

    beginInsertRows( QModelIndex(), 0, rowCount() );

    int num = m_ecl->getNumberEclYear();
    for( int i = 1; i <= num; ++i ) {
        EclipsesItem *item = new EclipsesItem( m_ecl, i );
        addItem( item );
    }

    endInsertRows();
}

} // namespace Marble

#include "moc_EclipsesModel.cpp"

