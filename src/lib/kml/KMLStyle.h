//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLSTYLE_H
#define KMLSTYLE_H

#include "KMLStyleSelector.h"
#include "KMLIconStyle.h"
#include "KMLLabelStyle.h"

class KMLStyle : public KMLStyleSelector
{
  public:
    KMLStyle();

    KMLIconStyle& getIconStyle();

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  private:
    KMLIconStyle m_iconStyle;
    KMLLabelStyle m_labelStyle;
};

#endif
