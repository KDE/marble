//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Guillaume Martres <smarter@ubuntu.com>
// Copyright 2011,2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef PLACEMARKPOSITIONPROVIDERPLUGIN_H
#define PLACEMARKPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include "GeoDataCoordinates.h"
#include "GeoDataAccuracy.h"

#include <QDateTime>

namespace Marble
{

class GeoDataPlacemark;
class MarbleModel;

class PlacemarkPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

 public:
    explicit PlacemarkPositionProviderPlugin( MarbleModel *marbleModel );

    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QVector<PluginAuthor> pluginAuthors() const override;
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
    void setPlacemark( const GeoDataPlacemark *placemark );

    void updatePosition();
};

}

#endif // PLACEMARKPOSITIONPROVIDERPLUGIN_H
