// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCSTATION_H
#define BBCSTATION_H

#include <QtGlobal>

class QString;

namespace Marble
{

class GeoDataCoordinates;
class BBCStationPrivate;

class BBCStation
{
public:
    BBCStation();
    BBCStation(const BBCStation &other);

    ~BBCStation();

    void detach();

    BBCStation &operator=(const BBCStation &other);
    bool operator<(const BBCStation &other) const;

    QString name() const;
    void setName(const QString &name);

    GeoDataCoordinates coordinate() const;
    void setCoordinate(const GeoDataCoordinates &coordinate);

    quint32 bbcId() const;
    void setBbcId(quint32 id);

    quint8 priority() const;
    void setPriority(quint8 priority);

private:
    BBCStationPrivate *d;
};

} // namespace Marble

#endif // BBCSTATION_H
