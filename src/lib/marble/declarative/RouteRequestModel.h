// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTEREQUESTMODEL_H
#define MARBLE_ROUTEREQUESTMODEL_H

#include <QAbstractListModel>

namespace Marble
{
class RouteRequest;
class Routing;
}

class RouteRequestModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Marble::Routing *routing READ routing WRITE setRouting NOTIFY routingChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

public:
    enum RouteRequestModelRoles { LongitudeRole = Qt::UserRole + 1, LatitudeRole = Qt::UserRole + 2 };

    /** Constructor */
    explicit RouteRequestModel(QObject *parent = nullptr);

    /** Destructor */
    ~RouteRequestModel() override;

    // Model querying

    /** Overload of QAbstractListModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /** Overload of QAbstractListModel */
    QHash<int, QByteArray> roleNames() const override;

    /** Overload of QAbstractListModel */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /** Overload of QAbstractListModel */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Marble::Routing *routing();

public Q_SLOTS:
    void setRouting(Marble::Routing *routing);

    void setPosition(int index, qreal longitude, qreal latitude);

Q_SIGNALS:
    void routingChanged();
    void rowCountChanged();

private Q_SLOTS:
    void updateMap();

    void updateData(int index);

    void updateAfterRemoval(int index);

    void updateAfterAddition(int index);

private:
    Marble::RouteRequest *m_request;
    Marble::Routing *m_routing;
    QHash<int, QByteArray> m_roleNames;
};

#endif // MARBLE_ROUTEREQUESTMODEL_H
