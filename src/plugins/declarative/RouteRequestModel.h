//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTEREQUESTMODEL_H
#define MARBLE_ROUTEREQUESTMODEL_H

#include <QtCore/QAbstractListModel>

namespace Marble { class RouteRequest; }

class RouteRequestModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RouteRequestModelRoles {
        LongitudeRole = Qt::UserRole+1,
        LatitudeRole = Qt::UserRole+2
    };

    /** Constructor */
    explicit RouteRequestModel( Marble::RouteRequest* request = 0, QObject *parent = 0 );

    /** Destructor */
    ~RouteRequestModel();

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

    /** Overload of QAbstractListModel */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

public Q_SLOTS:
    void setPosition ( int index, qreal longitude, qreal latitude );

private Q_SLOTS:
    void updateData( int index );

    void updateAfterRemoval( int index );

    void updateAfterAddition( int index );

private:
    Marble::RouteRequest* m_request;
};

#endif // MARBLE_ROUTEREQUESTMODEL_H
