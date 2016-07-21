//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Christian Ehrlicher <ch.ehrlicher@gmx.de>
//

#ifndef MARBLE_EXPORT_H
#define MARBLE_EXPORT_H

#include <QtGlobal>

#ifndef MARBLE_EXPORT
# ifdef MAKE_MARBLE_LIB
#  define MARBLE_EXPORT Q_DECL_EXPORT
# else
#  define MARBLE_EXPORT Q_DECL_IMPORT
# endif
#endif

#ifdef __GNUC__
#define MARBLE_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define MARBLE_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement MARBLE_DEPRECATED for this compiler in marble_export.h")
#define MARBLE_DEPRECATED(func) func
#endif

#endif // MARBLE_EXPORT_H
