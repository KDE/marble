//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef PLACEMARKPOSITIONPROVIDERPLUGIN_H
#define PLACEMARKPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

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
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    virtual PositionProviderPlugin *newInstance() const;

    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;

    void setPlacemark( const GeoDataPlacemark *placemark );

 private:
    QTimer *m_updateTimer;
    const GeoDataPlacemark *m_placemark;

    PositionProviderStatus m_status;
    GeoDataAccuracy m_accuracy;
    bool m_isInitialized;

    void update();

 private slots:
    void updatePosition();
};

}

#endif // PLACEMARKPOSITIONPROVIDERPLUGIN_H
