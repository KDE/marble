// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCLUEPOSITIONPROVIDERPLUGIN_H
#define GEOCLUEPOSITIONPROVIDERPLUGIN_H

#include "GeoCute/Position.h"
#include "GeoCute/Status.h"

#include "PositionProviderPlugin.h"



namespace GeoCute
{
    class PositionProvider;
}

namespace Marble
{

class GeoCluePositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GeoCluePositionProviderPlugin")
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

 public:
    GeoCluePositionProviderPlugin();
    virtual ~GeoCluePositionProviderPlugin();

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
    GeoCute::PositionProvider* m_positionProvider;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    
 private Q_SLOTS:
    void updatePosition(GeoCute::Position newPosition);
    void updateStatus(GeoCute::Status newStatus);
};

}



#endif
