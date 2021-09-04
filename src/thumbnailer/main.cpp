// SPDX-FileCopyrightText: 2014 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "thumbnailer.h"


extern "C"
{

Q_DECL_EXPORT ThumbCreator *new_creator()
{
    return new Marble::GeoDataThumbnailer;
}

}
