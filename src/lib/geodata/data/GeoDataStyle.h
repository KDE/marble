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

#include <QtCore/QMetaType>
#include <QtCore/QPoint>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QFont>

#include "GeoDataStyleSelector.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"

class GeoDataStyle : public GeoDataStyleSelector
{
  public:
    GeoDataStyle();
    GeoDataStyle( const QPixmap& icon, 
                  const QFont &font, const QColor &color  );
    ~GeoDataStyle();

    GeoDataIconStyle*  iconStyle();
    GeoDataLabelStyle* labelStyle();

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataIconStyle  *m_iconStyle;
    GeoDataLabelStyle *m_labelStyle;
};

Q_DECLARE_METATYPE( GeoDataStyle* )

#endif // GEODATASTYLE_H
