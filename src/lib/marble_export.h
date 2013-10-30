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


#include <QtPlugin>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    #if defined(Q_EXPORT_PLUGIN)
        #undef Q_EXPORT_PLUGIN
        #undef Q_EXPORT_PLUGIN2
    #endif

    #define Q_EXPORT_PLUGIN(a)
    #define Q_EXPORT_PLUGIN2(a, b)
#else
    # define Q_PLUGIN_METADATA(a)
#endif

#endif // MARBLE_EXPORT_H
