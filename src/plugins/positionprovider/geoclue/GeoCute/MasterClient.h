//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_MASTERCLIENT_H
#define GEOCUTE_MASTERCLIENT_H

#include <QtCore/QObject>

#include "AccuracyLevel.h"



namespace GeoCute
{
    
enum ResourceFlags {
    ResourceNone = 0,
    ResourceNetwork = 1 << 0,
    ResourceCell = 1 << 1,
    ResourceGPS = 1 << 2,
    ResourceAll = (1 << 10) - 1
};

enum SignallingFlags {
    SignallingNotRequired,
    SignallingRequired
};

class PositionProvider;

class MasterClient : public QObject
{
    Q_OBJECT
    
    public:
        MasterClient(QObject* parent = 0);
        ~MasterClient();
        PositionProvider* positionProvider();
        void setRequirements(AccuracyLevel, int min_time, SignallingFlags,
            ResourceFlags);
            
    private:
        class Private;
        Private* const d;
};

}



#endif
