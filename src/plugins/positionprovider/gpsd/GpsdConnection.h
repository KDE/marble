// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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
    explicit GpsdConnection( QObject* parent = nullptr );

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
