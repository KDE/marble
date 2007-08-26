//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLCOLORSTYLE_H
#define KMLCOLORSTYLE_H

#include <QtGui/QColor>
#include "KMLObject.h"

class KMLColorStyle : public KMLObject
{
  public:
    void setColor( const QColor &value );
    QColor color() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  protected:
    KMLColorStyle();

  protected:
    QColor m_color;
};

#endif
