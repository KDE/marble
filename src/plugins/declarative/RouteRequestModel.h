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

#include <QAbstractListModel>

namespace Marble { class RouteRequest; }
class Routing;

class RouteRequestModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( Routing* routing READ routing WRITE setRouting NOTIFY routingChanged )
    Q_PROPERTY( int count READ rowCount )

public:
    enum RouteRequestModelRoles {
        LongitudeRole = Qt::UserRole+1,
        LatitudeRole = Qt::UserRole+2
    };

    /** Constructor */
    explicit RouteRequestModel( QObject *parent = 0 );

    /** Destructor */
    ~RouteRequestModel();

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

#if QT_VERSION >= 0x050000
    /** Overload of QAbstractListModel */
    QHash<int, QByteArray> roleNames() const;
#endif

    /** Overload of QAbstractListModel */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    Routing *routing();

public Q_SLOTS:
    void setRouting( Routing *routing );

    void setPosition ( int index, qreal longitude, qreal latitude );

Q_SIGNALS:
    void routingChanged();

private Q_SLOTS:
    void updateMap();

    void updateData( int index );

    void updateAfterRemoval( int index );

    void updateAfterAddition( int index );

private:
    Marble::RouteRequest* m_request;
    Routing *m_routing;
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> m_roleNames;
#endif
};

#endif // MARBLE_ROUTEREQUESTMODEL_H
