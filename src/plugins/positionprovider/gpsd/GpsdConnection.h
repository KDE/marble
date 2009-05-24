//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDCONNECTION_H
#define GPSDCONNECTION_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <libgpsmm.h>



namespace Marble
{

class GpsdConnection : public QObject
{
    Q_OBJECT

 public:
    GpsdConnection( QObject* parent = 0 );

 signals:
    void gpsdInfo( gps_data_t data );
    
 private:
    gpsmm m_gpsd;
    QTimer m_timer;
    
 private slots:
    void update();
};

}



#endif
