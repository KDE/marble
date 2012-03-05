//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDPOSITIONPROVIDERPLUGIN_H
#define GPSDPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include <libgpsmm.h>



namespace Marble
{

class GpsdThread;

class GpsdPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

 public:
    GpsdPositionProviderPlugin();
    virtual ~GpsdPositionProviderPlugin();

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

    virtual PositionProviderPlugin * newInstance() const;

    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;
    virtual QString error() const;
    virtual qreal speed() const;
    virtual qreal direction() const;

 private:
    GpsdThread* m_thread;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    qreal m_speed;
    qreal m_track;

 private slots:
    void update(gps_data_t data);
};

}

#endif
