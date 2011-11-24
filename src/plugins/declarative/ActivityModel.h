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

/**
 * Model to access activities from QML.
 */
class ActivityModel : public QAbstractListModel
{

    Q_OBJECT

 public:
    /**
     * The roles of the activities.
     */
    enum Roles
    {
      NameRole = Qt::UserRole + 1,     ///< Name of the activity.
      ImagePathRole,                   ///< Path to image for activity.
      PathRole                        ///< Path to QML file for activity.
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
    /**
     * Add an activity with the passed data.
     * 
     * @param name Name of the activity
     * @param imagePath Path to the image of the activity.
     * @param path Path to the QML file of the activity.
     * 
     * @see Activity
     */
    void addActivity( const QString& name, const QString& imagePath, const QString &path );
    /**
     * Remove activity with the passed name.
     *
     * @param name Name of the activity to remove.
     * 
     * @see Roles
     */
    void removeActivity( const QString& name );
    
    /**
     * Returns data of activity with passed index and role.
     *
     * @param index Index of the activity in the model.
     * @param role Role of the requested data in the model.
     * 
     * @return Data of activity at passed index and role.
     * 
     * @see Roles
     */
    QVariant get( const int index, const QString& role ) const;
    
    /**
     * Returns data of activity with passed name and role.
     *
     * @param name Name of the activity in the model.
     * @param role Role of the requested data in the model.
     * 
     * @return Data of activity with passed name and role.
     * 
     * @see Roles
     */
    QVariant get( const QString& name, const QString& role ) const;

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
