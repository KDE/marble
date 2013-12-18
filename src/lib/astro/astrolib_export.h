//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Marek Hakala <hakala.marek@gmail.com>
//

#ifndef ASTROLIB_EXPORT_H
#define ASTROLIB_EXPORT_H

#include <QtGlobal>

#ifndef ASTROLIB_EXPORT
# ifdef MAKE_ASTRO_LIB
#  define ASTROLIB_EXPORT Q_DECL_EXPORT
# else
#  define ASTROLIB_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // ASTROLIB_EXPORT_H
