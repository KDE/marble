//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#ifndef MARBLE_ACTIVITYMODEL_H
#define MARBLE_ACTIVITYMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

namespace Marble
{
    
namespace Declarative
{

class ActivityModel : public QAbstractListModel
{

    Q_OBJECT

 public:
    /**
     * The roles of the activities.
     */
    enum Roles
    {
      NameRole = Qt::UserRole + 1,     ///< Name of the activity
      ImagePathRole,                   ///< Path to image for activity
      EnablePluginsRole,               ///< Plugins to enable when switching to that activity
      DisablePluginsRole,              ///< Plugins to disable when switching to that activity
      RelatedActivitiesRole            ///< Related activities and plugins to preserve when switching to them
    };

    explicit ActivityModel( QObject *parent = 0 );

    ~ActivityModel();

    /**
     * Return the data according to the index.
     *
     * @param index  the index of the data
     * @param role   which part of the data to return.  @see Roles
     */
    QVariant data( const QModelIndex &index, int role ) const;

 public Q_SLOTS:
    void addActivity( const QString& name, const QString& imagePath, 
                      const QStringList& enablePlugins, const QStringList& disablePlugins,
                      const QVariant& relatedActivities );
    void removeActivity( const QString& name );
    QVariant get( const int index, const QString& role ) const;
    /**
     * Return the number of activities in the Model.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    
 private:
    Q_DISABLE_COPY( ActivityModel )
    class Private;
    Private* const d;
};

}

}

#endif
