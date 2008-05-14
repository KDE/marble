//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
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

#include "geodata_export.h"

class GeoDataStylePrivate;

/**
 * @short an addressable style group
 *
 * A GeoDataStyle defines an addressable style group that can be
 * referenced by GeoDataStyleMaps and GeoDataFeatures. GeoDataStyles
 * affect how Geometry is presented in the 3D viewer (not yet
 * implemented) and how Features appear. Shared styles are collected
 * in a GeoDataDocument and must have an id defined for them so that
 * they can be referenced by the individual Features that use them.
 *
 * @see GeoDataIconStyle
 * @see GeoDataLabelStyle
 */
class GEODATA_EXPORT GeoDataStyle : public GeoDataStyleSelector
{
  public:
    /// Construct a default style
    GeoDataStyle();
    /**
     * @brief Construct a new style
     * @param  icon   used to construct the icon style
     * @param  font   used to construct the label styls
     * @param  color  used to construct the label styls
     */
    GeoDataStyle( const QPixmap& icon, 
                  const QFont &font, const QColor &color  );
    ~GeoDataStyle();

    /// Return the icon style of this style
    GeoDataIconStyle*  iconStyle();
    /// Return the label style of this style
    GeoDataLabelStyle* labelStyle();

    /**
     * @brief Serialize the style to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataStylePrivate * const d;
};

Q_DECLARE_METATYPE( GeoDataStyle* )

#endif // GEODATASTYLE_H
