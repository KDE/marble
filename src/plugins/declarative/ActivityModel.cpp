//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#include "ActivityModel.h"
#include "Activity.h"

#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeProperty>

namespace Marble
{
    
namespace Declarative
{

class ActivityModel::Private
{

 public:
    Private()
    {
    }

    ~Private()
    {
    }

    QList<Activity*>     m_activityContainer;     ///< Container to store activities.
};


// ---------------------------------------------------------------------------


ActivityModel::ActivityModel( QObject *parent )
    : QAbstractListModel( parent ),
      d( new Private )
{
    // Setup roles for activities.
    QHash<int,QByteArray> roles = roleNames();
    roles[NameRole] = "name";
    roles[ImagePathRole] = "imagePath";
    roles[PathRole] = "path";
    roles[PageRole] = "page";
    setRoleNames( roles );
}

ActivityModel::~ActivityModel()
{
    delete d;
}

void ActivityModel::addActivity( const QString& name, const QString& imagePath, QObject* page,
                                 const QString &path )
{
    d->m_activityContainer.push_back( new Activity( name, imagePath, page, path ) );
}

void ActivityModel::removeActivity( const QString& name )
{
    for( int i = 0; i < d->m_activityContainer.size(); i++ ) {
        if( name == d->m_activityContainer[i]->name() ) {
            delete d->m_activityContainer.takeAt( i );
            i--;
        }
    }
}

int ActivityModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent )
    return d->m_activityContainer.size();
}

int ActivityModel::columnCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return 1;
    else
        return 0;
}

void ActivityModel::load( QObject *parent, QObject* child )
{
    /** @todo: Is doing this here in the model the best place? Rename the method for clarity at least */
    QDeclarativeItem* parentItem = qobject_cast<QDeclarativeItem*>( parent );
    if ( parentItem ) {
        QDeclarativeProperty( parentItem, "marbleWidget" ).write( qVariantFromValue( child ) );
    }
}

QVariant ActivityModel::data( const QModelIndex &index, int role ) const
{
    // Check if passed index is valid.
    if ( !index.isValid() )
        return QVariant();

    // Check if index is out of bound.
    if ( index.row() >= d->m_activityContainer.size() )
        return QVariant();

    // Return the requested data, an empty QVariant for an unknown role.
    if ( role == NameRole ) {
        return d->m_activityContainer.at( index.row() )->name();
    } else if( role == ImagePathRole ) {
        return d->m_activityContainer.at( index.row() )->imagePath();
    } else if( role == PathRole ) {
        return d->m_activityContainer.at( index.row() )->path();
    } else if ( role == PageRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->page() );
    } else
        return QVariant();
}

QVariant ActivityModel::get( const int index, const QString& role ) const
{
    // Check if index is out of bound.
    if ( index < 0 || index > d->m_activityContainer.size() )
        return QVariant();

    static QHash<int,QByteArray> roles = roleNames();

    // Return the requested data, an empty QVariant for an unknown role.
    if ( role == roles[NameRole] ) {
        return d->m_activityContainer.at( index )->name();
    } else if( role == roles[ImagePathRole] ) {
        return d->m_activityContainer.at( index )->imagePath();
    } else if( role == roles[PathRole] ) {
        return d->m_activityContainer.at( index )->path();
    } else if ( role == roles[PageRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->page() );
    } else
        return QVariant();
}

QVariant ActivityModel::get( const QString& name, const QString& role ) const
{
    // Check for valid activity name.
    if( name.isEmpty() )
        return QVariant();
    
    static QHash<int,QByteArray> roles = roleNames();
    Activity *activity = 0;
    
    // Get a pointer to the activity with the passed name.
    for( int i = 0; i < d->m_activityContainer.size(); i++ ) {
        if ( d->m_activityContainer[i]->name() == name ) {
            activity = d->m_activityContainer.at( i );
        }
    }
    // Return requested data if activity is valid, an empty QVariant
    // for an unknown activity or role.
    if( activity != 0 ) {
        if ( role == roles[NameRole] ) {
            return activity->name();
        } else if( role == roles[ImagePathRole] ) {
            return activity->imagePath();
        } else if( role == roles[PathRole] ) {
            return activity->path();
        } else if( role == roles[PageRole] ) {
            return qVariantFromValue( activity->page() );
        } else
            return QVariant();
    }
    else
        return QVariant();
}

}

}

#include "ActivityModel.moc"
