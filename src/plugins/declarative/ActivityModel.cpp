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
    roles[PathRole] = "path";
    roles[EnablePluginsRole] = "enablePlugins";
    roles[DisablePluginsRole] = "disablePlugins";
    roles[RelatedActivitiesRole] = "relatedActivities";
    roles[SettingsRole] = "settings";
    setRoleNames( roles );
}

ActivityModel::~ActivityModel()
{
    delete d;
}

void ActivityModel::addActivity( const QString& name, const QString& imagePath, const QString& path,
                                 const QStringList& enablePlugins, const QStringList& disablePlugins,
                                 const QVariant& relatedActivities, const QVariant& settings )
{
    d->m_activityContainer.push_back( new Activity( name, imagePath, path, enablePlugins, disablePlugins, 
                                                    relatedActivities.value<QMap<QString, QVariant> >(),
                                                    settings.value<QMap<QString, QVariant> >()
                                                  ) );
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
    } else if( role == PathRole ) {
        return d->m_activityContainer.at( index.row() )->path();
    } else if ( role == EnablePluginsRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->enablePlugins() );
    } else if ( role == DisablePluginsRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->disablePlugins() );
    } else if ( role == RelatedActivitiesRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->relatedActivities() );
    } else if ( role == RelatedActivitiesRole ) {
        return qVariantFromValue( d->m_activityContainer.at( index.row() )->settings() );
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
    } else if( role == roles[PathRole] ) {
        return d->m_activityContainer.at( index )->path();
    } else if ( role == roles[EnablePluginsRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->enablePlugins() );
    } else if ( role == roles[DisablePluginsRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->disablePlugins() );
    } else if ( role == roles[RelatedActivitiesRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->relatedActivities() );
    } else if ( role == roles[SettingsRole] ) {
        return qVariantFromValue( d->m_activityContainer.at( index )->settings() );
    } else
        return QVariant();
}

QVariant ActivityModel::get( const QString& name, const QString& role ) const
{
    if( name.isEmpty() )
        return QVariant();
    
    static QHash<int,QByteArray> roles = roleNames();
    Activity *activity = 0;
    
    for( int i = 0; i < d->m_activityContainer.size(); i++ ) {
        if ( d->m_activityContainer[i]->name() == name ) {
            activity = d->m_activityContainer.at( i );
        }
    }
    if( activity != 0 ) {
        if ( role == roles[NameRole] ) {
            return activity->name();
        } else if( role == roles[ImagePathRole] ) {
            return activity->imagePath();
        } else if( role == roles[PathRole] ) {
            return activity->path();
        } else if ( role == roles[EnablePluginsRole] ) {
            return activity->enablePlugins();
        } else if ( role == roles[DisablePluginsRole] ) {
            return activity->disablePlugins();
        } else if ( role == roles[RelatedActivitiesRole] ) {
            return activity->relatedActivities();
        } else if( role == roles[SettingsRole] ) {
            return activity->settings();
        } else
            return QVariant();
    }
    else
        return QVariant();
}

}

}

#include "ActivityModel.moc"
