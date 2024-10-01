// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
// SPDX-FileCopyrightText: 2011, 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef PLACEMARKPOSITIONPROVIDERPLUGIN_H
#define PLACEMARKPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include "GeoDataAccuracy.h"
#include "GeoDataCoordinates.h"

#include <QDateTime>

namespace Marble
{

class GeoDataPlacemark;
class MarbleModel;

class PlacemarkPositionProviderPlugin : public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES(Marble::PositionProviderPluginInterface)

public:
    explicit PlacemarkPositionProviderPlugin(MarbleModel *marbleModel, QObject *parent = nullptr);

    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QList<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    void initialize() override;
    bool isInitialized() const override;

    PositionProviderPlugin *newInstance() const override;

    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    GeoDataAccuracy accuracy() const override;
    qreal speed() const override;
    qreal direction() const override;
    QDateTime timestamp() const override;

private:
    MarbleModel *const m_marbleModel;
    const GeoDataPlacemark *m_placemark;
    GeoDataCoordinates m_coordinates;
    QDateTime m_timestamp;
    qreal m_speed;
    qreal m_direction;

    PositionProviderStatus m_status;
    GeoDataAccuracy m_accuracy;
    bool m_isInitialized;

private Q_SLOTS:
    void setPlacemark(const GeoDataPlacemark *placemark);

    void updatePosition();
};

}

#endif // PLACEMARKPOSITIONPROVIDERPLUGIN_H
