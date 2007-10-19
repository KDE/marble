//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATACOLORSTYLE_H
#define GEODATACOLORSTYLE_H

#include <QtGui/QColor>
#include "GeoDataObject.h"

class GeoDataColorStyle : public GeoDataObject
{
  public:
    void setColor( const QColor &value );
    QColor color() const;

    enum ColorMode{Normal, Random};

    void setColorMode( const ColorMode &colorMode );
    ColorMode colorMode() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  protected:
    GeoDataColorStyle();

  protected:
    QColor m_color;
    ColorMode m_colorMode;
};

#endif // GEODATACOLORSTYLE_H
