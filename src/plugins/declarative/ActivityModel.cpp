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
#include "MarbleDebug.h"

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

    QList<Activity*>     m_activityContainer;
};


// ---------------------------------------------------------------------------


ActivityModel::ActivityModel( QObject *parent )
    : QAbstractListModel( parent ),
      d( new Private )
{
    QHash<int,QByteArray> roles = roleNames();
    roles[NameRole] = "name";
    roles[ImagePathRole] = "imagePath";
    roles[EnablePluginsRole] = "enablePlugins";
    roles[DisablePluginsRole] = "disablePlugins";
    roles[RelatedActivitiesRole] = "relatedActivities";
    setRoleNames( roles );
}

ActivityModel::~ActivityModel()
{
    delete d;
}

void ActivityModel::addActivity( const QString& name, const QString& imagePath, 
                                 const QStringList& enablePlugins, const QStringList& disablePlugins,
                                 const QVariant& relatedActivities )
{
    d->m_activityContainer.push_back( new Activity( name, imagePath, enablePlugins, disablePlugins, 
                                                    relatedActivities.value<QMap<QString, QVariant> >() ) );
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
    return d->m_activityContainer.size();
}

int ActivityModel::columnCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return 1;
    else
        return 0;
}

QVariant ActivityModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= d->m_activityContainer.size() )
        return QVariant();

    if ( role == NameRole ) {
        return d->m_activityContainer.at( index.row() )->name();
    } else if( role == ImagePathRole ) {
        return d->m_activityContainer.at( index.row() )->imagePath();
    } else if ( role == EnablePluginsRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->enablePlugins() );
    } else if ( role == DisablePluginsRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->disablePlugins() );
    } else if ( role == RelatedActivitiesRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->relatedActivities() );
    } else
        return QVariant();
}

QVariant ActivityModel::get( const int index, const QString& role ) const
{
    if ( index < 0 || index > d->m_activityContainer.size() )
        return QVariant();

    static QHash<int,QByteArray> roles = roleNames();
    
    if ( role == roles[NameRole] ) {
        return d->m_activityContainer.at( index )->name();
    } else if( role == roles[ImagePathRole] ) {
        return d->m_activityContainer.at( index )->imagePath();
    } else if ( role == roles[EnablePluginsRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->enablePlugins() );
    } else if ( role == roles[DisablePluginsRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->disablePlugins() );
    } else if ( role == roles[RelatedActivitiesRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->relatedActivities() );
    } else
        return QVariant();
}

}

}

#include "ActivityModel.moc"
