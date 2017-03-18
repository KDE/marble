//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDCONNECTION_H
#define GPSDCONNECTION_H

#include "PositionProviderPlugin.h"

#include <QObject>
#include <QTimer>

#include <libgpsmm.h>

namespace Marble
{

class GpsdConnection : public QObject
{
    Q_OBJECT

 public:
    explicit GpsdConnection( QObject* parent = 0 );

    ~GpsdConnection() override;

    void initialize();

    QString error() const;    

 Q_SIGNALS:
    void gpsdInfo( gps_data_t data );

    void statusChanged( PositionProviderStatus status ) const;    

private:
    void open();

    gpsmm m_gpsd;
    QTimer m_timer;
    PositionProviderStatus m_status;
    QString m_error;
    const char* m_oldLocale;
    
 private Q_SLOTS:
    void update();
};

}

#endif
