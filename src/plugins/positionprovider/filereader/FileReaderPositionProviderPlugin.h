//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
//

#ifndef GPS_SIMULATION_PLUGIN_H
#define GPS_SIMULATION_PLUGIN_H

#include "PositionProviderPlugin.h"

class QTimer;

namespace Marble
{

class GpsSimulationPluginPrivate;

class GpsSimulationPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    GpsSimulationPlugin();
    virtual ~GpsSimulationPlugin();

    // Implementing PluginInterface
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;
    virtual qreal speed() const;
    virtual qreal direction() const;

    // Implementing PositionProviderPlugin
    virtual PositionProviderPlugin * newInstance() const;

    // Implementing PositionProviderPluginInterface
    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;

private Q_SLOTS:
    /** Regular (each second) position and status update */
    void update();

public Q_SLOTS:
	void positionProviderDisabled(bool);


private:
    GpsSimulationPluginPrivate* const d;
	QTimer*			m_pUpdateTimer;

	qreal m_lon;
	qreal m_lat;

	int   m_iPos;

};

}

#endif // QT_POSITION_PROVIDER_PLUGIN_H

