//
// This file is part of the Marble Desktop Globe.
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

#include <QtCore/QTimer>

#include <libgpsmm.h>



namespace Marble
{

class GpsdPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT

 public:
    GpsdPositionProviderPlugin();
    virtual ~GpsdPositionProviderPlugin();

    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    virtual PositionProviderPlugin * newInstance() const;

    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;
    
 private:
    bool m_initialized;
    gpsmm* m_gpsd;
    gps_data_t* m_gpsdData;
    QTimer* m_timer;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    
 private slots:
    void update();
};

}



#endif
