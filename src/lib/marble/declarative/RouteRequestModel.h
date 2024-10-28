// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTEREQUESTMODEL_H
#define MARBLE_ROUTEREQUESTMODEL_H

#include <QAbstractListModel>
#include <qqmlregistration.h>

namespace Marble
{
class RouteRequest;
class Routing;
}

class RouteRequestModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Marble::Routing *routing READ routing WRITE setRouting NOTIFY routingChanged)

public:
    enum RouteRequestModelRoles {
        LongitudeRole = Qt::UserRole + 1,
        LatitudeRole = Qt::UserRole + 2
    };

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

    void setPosition(int index, qreal longitude, qreal latitude, const QString &name);

    void reverse();

Q_SIGNALS:
    void routingChanged();

private Q_SLOTS:
    void updateMap();

    void updateData(int index);

    void updateAfterRemoval(int index);

    void updateAfterAddition(int index);

private:
    Marble::RouteRequest *m_request = nullptr;
    Marble::Routing *m_routing = nullptr;
};

#endif // MARBLE_ROUTEREQUESTMODEL_H
