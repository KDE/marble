//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLICONSTYLE_H
#define KMLICONSTYLE_H

#include <QtGui/QIcon>
#include "KMLColorStyle.h"

class KMLIconStyle : public KMLColorStyle
{
  public:
    KMLIconStyle();

    void setIcon( QIcon value );
    QIcon icon() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  private:
    QIcon m_icon;
};

#endif
