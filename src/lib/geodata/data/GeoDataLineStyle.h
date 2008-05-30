//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef GEODATALINESTYLE_H
#define GEODATALINESTYLE_H


#include <QtGui/QFont>

#include "GeoDataColorStyle.h"

#include "geodata_export.h"

class GeoDataLineStylePrivate;

/**
 * @short specifies the style how lines are drawn
 *
 * A GeoDataLineStyle specifies how the <b>name</b> of a
 * GeoDataFeature is drawn in the viewer. A custom color, color mode
 * (both inherited from GeoDataColorStyle) and width for the width
 * of the line.
 */
class GEODATA_EXPORT GeoDataLineStyle : public GeoDataColorStyle
{
  public:
    /// Construct a new GeoDataLineStyle
    GeoDataLineStyle();
    /**
     * @brief Construct a new GeoDataLineStyle
     * @param  color  the color to use when showing the name @see GeoDataColorStyle
     */
    explicit GeoDataLineStyle( const QColor &color );

    ~GeoDataLineStyle();

    /**
     * @brief Set the width of the line
     * @param  width  the new width
     */
    void setWidth( const float &width );
    /**
     * @brief Return the current width of the line
     * @return the current width
     */
    float width() const;

    /**
     * @brief  Serialize the style to a stream.
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  private:
    Q_DISABLE_COPY( GeoDataLineStyle )
    GeoDataLineStylePrivate * const d;
};

#endif // GeoDataLineStyle_H
