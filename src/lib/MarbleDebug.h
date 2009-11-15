#ifndef MARBLEDEBUG_H
#define MARBLEDEBUG_H

#include <QDebug>

#include "marble_export.h"

namespace Marble {
/**
  * a simple null device
  */
class NullDevice : public QIODevice {
    public:
        qint64 QIODevice::readData ( char * data, qint64 maxSize ) { return -1; };
        qint64 QIODevice::writeData ( const char * data, qint64 maxSize ) { return maxSize; };
};

/**
  * a class which takes all the settings and exposes them
  */

class MARBLE_EXPORT MarbleDebug {
    public:
        static bool enable;
        static QIODevice* nullDevice() { static QIODevice *device = new NullDevice; return device; };
};

/**
  * an inline function which should replace qDebug()
  */

inline QDebug mDebug() { 
    if(MarbleDebug::enable) 
        return QDebug(QtDebugMsg); 
    else 
        return QDebug(MarbleDebug::nullDevice()); 
}

} // namespace Marble

#endif /* MARBLEDEBUG_H */