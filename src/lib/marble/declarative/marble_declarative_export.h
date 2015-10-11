//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Christian Ehrlicher <ch.ehrlicher@gmx.de>
//

#ifndef MARBLE_DECLARATIVE_EXPORT_H
#define MARBLE_DECLARATIVE_EXPORT_H

#include <QtGlobal>

#ifndef MARBLE_DECLARATIVE_EXPORT
# ifdef MAKE_MARBLE_DECLARATIVE_LIB
#  define MARBLE_DECLARATIVE_EXPORT Q_DECL_EXPORT
# else
#  define MARBLE_DECLARATIVE_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // MARBLE_DECLARATIVE_EXPORT_H
