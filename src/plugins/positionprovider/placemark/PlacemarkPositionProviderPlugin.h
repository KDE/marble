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

#include <QtCore/QDateTime>

namespace Marble
{

class GeoDataPlacemark;


class PlacemarkPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

 public:
    PlacemarkPositionProviderPlugin();

    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString version() const;
    virtual QString description() const;
    virtual QString copyrightYears() const;
    virtual QList<PluginAuthor> pluginAuthors() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    virtual PositionProviderPlugin *newInstance() const;

    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;
    virtual qreal speed() const;
    virtual qreal direction() const;
    virtual QDateTime timestamp() const;

 private:
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
