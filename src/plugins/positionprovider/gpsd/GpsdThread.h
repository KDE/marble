//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDTHREAD_H
#define GPSDTHREAD_H

#include "PositionProviderPlugin.h"

#include <QtCore/QThread>

#include <libgpsmm.h>



namespace Marble
{

class GpsdConnection;

class GpsdThread: public QThread
{
    Q_OBJECT

 public:
    GpsdThread();

    ~GpsdThread();

    virtual void run();

    QString error() const;

Q_SIGNALS:
    void statusChanged( PositionProviderStatus status ) const;

private:
    GpsdConnection* m_connection;
    
 signals:
    void gpsdInfo( gps_data_t data );
};

}

#endif
