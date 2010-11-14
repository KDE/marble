//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Christian Ehrlicher <ch.ehrlicher@gmx.de>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATA_EXPORT_H
#define GEODATA_EXPORT_H

#include <QtCore/QtGlobal>

// please make sure to change MAKE_MARBLE_LIB to MAKE_GEODATA_LIB if geodata is an independent library
#ifndef GEODATA_EXPORT
# ifdef MAKE_MARBLE_LIB
#  define GEODATA_EXPORT Q_DECL_EXPORT
# else
#  define GEODATA_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // GEODATA_EXPORT_H
