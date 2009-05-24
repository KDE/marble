//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDTHREAD_H
#define GPSDTHREAD_H

#include <QtCore/QThread>

#include <libgpsmm.h>



namespace Marble
{

class GpsdThread: public QThread
{
    Q_OBJECT

 public:
    virtual void run();
    
 signals:
    void gpsdInfo( gps_data_t data );
};

}



#endif
