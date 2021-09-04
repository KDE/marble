// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDTHREAD_H
#define GPSDTHREAD_H

#include "PositionProviderPlugin.h"

#include <QThread>

#include <libgpsmm.h>



namespace Marble
{

class GpsdConnection;

class GpsdThread: public QThread
{
    Q_OBJECT

 public:
    GpsdThread();

    ~GpsdThread() override;

    void run() override;

    QString error() const;

Q_SIGNALS:
    void statusChanged( PositionProviderStatus status ) const;

private:
    GpsdConnection* m_connection;
    
 Q_SIGNALS:
    void gpsdInfo( gps_data_t data );
};

}

#endif
