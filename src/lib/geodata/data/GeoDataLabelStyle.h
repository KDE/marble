//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATALABELSTYLE_H
#define GEODATALABELSTYLE_H

#include "GeoDataColorStyle.h"

#include <QtGui/QFont>

class GeoDataLabelStyle : public GeoDataColorStyle
{
  public:
    GeoDataLabelStyle();
    GeoDataLabelStyle( const QFont &font, const QColor &color );

    void setScale( const float &scale );
    float scale() const;

    void setFont( const QFont &font );
    QFont font() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  protected:
    float m_scale;

    QFont m_font;   // No KML property
};

#endif // GEODATALABELSTYLE_H
