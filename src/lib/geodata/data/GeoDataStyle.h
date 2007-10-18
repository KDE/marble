//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATASTYLE_H
#define GEODATASTYLE_H

#include "GeoDataStyleSelector.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"

class GeoDataStyle : public GeoDataStyleSelector
{
  public:
    GeoDataStyle();

    GeoDataIconStyle& getIconStyle();

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataIconStyle m_iconStyle;
    GeoDataLabelStyle m_labelStyle;
};

#endif // GEODATASTYLE_H
