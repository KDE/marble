//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
//


#ifndef MARBLE_GEODATASTYLE_H
#define MARBLE_GEODATASTYLE_H


#include <QtCore/QMetaType>
#include <QtCore/QPoint>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QFont>

#include "GeoDataStyleSelector.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataLineStyle.h"

#include "geodata_export.h"

namespace Marble
{

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
 * @see GeoDataLineStyle
 * @see GeoDataPolyStyle
 */
class GEODATA_EXPORT GeoDataStyle : public GeoDataStyleSelector
{
  public:
    /// Construct a default style
    GeoDataStyle();
    GeoDataStyle( const GeoDataStyle& other );
    /**
     * @brief Construct a new style
     * @param  icon   used to construct the icon style
     * @param  font   used to construct the label styles
     * @param  color  used to construct the label styles
     */
    GeoDataStyle( const QPixmap& icon, 
                  const QFont &font, const QColor &color  );
    ~GeoDataStyle();

    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const;

    /// set the icon style
    void setIconStyle( const GeoDataIconStyle& style );
    /// Return the icon style of this style
    GeoDataIconStyle& iconStyle() const;
    /// set the label style
    void setLabelStyle( const GeoDataLabelStyle& style );
    /// Return the label style of this style
    GeoDataLabelStyle& labelStyle() const;
    /// set the line style
    void setLineStyle( const GeoDataLineStyle& style );
    /// Return the label style of this style
    GeoDataLineStyle& lineStyle() const;
    /// set the poly style
    void setPolyStyle( const GeoDataPolyStyle& style );
    /// Return the label style of this style
    GeoDataPolyStyle& polyStyle() const;

    /**
    * @brief assignment operator
    * @param other the GeoDataStyle that gets duplicated
    */
    GeoDataStyle& operator=( const GeoDataStyle& other );

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

}

Q_DECLARE_METATYPE( Marble::GeoDataStyle* )

#endif
