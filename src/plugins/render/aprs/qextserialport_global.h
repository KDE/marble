//
// A note from Brandon Fosdick, one of the QExtSerialPort developers,
// to Wes Hardaker states:
//
// qesp doesn't really have a license, it's considered completely public
// domain.
//

#ifndef QEXTSERIALPORT_GLOBAL_H
#define QEXTSERIALPORT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QEXTSERIALPORT_LIB
# define QEXTSERIALPORT_EXPORT Q_DECL_EXPORT
#else
# define QEXTSERIALPORT_EXPORT Q_DECL_IMPORT
#endif

#endif // QEXTSERIALPORT_GLOBAL_H

