//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLLABELSTYLE_H
#define KMLLABELSTYLE_H

#include "KMLColorStyle.h"

class KMLLabelStyle : public KMLColorStyle
{
  public:
    KMLLabelStyle();

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );
};

#endif
