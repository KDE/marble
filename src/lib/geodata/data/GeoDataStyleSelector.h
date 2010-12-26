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


#ifndef MARBLE_GEODATASTYLESELECTOR_H
#define MARBLE_GEODATASTYLESELECTOR_H


#include <QtCore/QString>

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataStyleSelectorPrivate;

/**
 * @short a base class for the style classes
 *
 * A GeoDataStyleSelector is a base class for the style classes Style
 * and StyleMap classes. The StyleMap class selects a style based on
 * the current mode of a Placemark (highlighted or normal). An element
 * derived from StyleSelector is uniquely identified by its id and its
 * url.
 *
 * @See GeoDataStyle
 * @See GeoDataStyleMap
 */
class GEODATA_EXPORT GeoDataStyleSelector : public GeoDataObject
{
  public:
    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const;

    /**
     * @brief Set a new style id.
     * @param  value  the new id
     */
    void setStyleId( const QString &value );
    /**
     * @brief Return the style id.
     * @return the style id
     */
    QString styleId() const;

    /**
    * @brief assignment operator
    */
    GeoDataStyleSelector& operator=( const GeoDataStyleSelector& other );

    /**
     * @brief Serialize the styleselector to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the styleselector from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

    GeoDataStyleSelector();
    GeoDataStyleSelector( const GeoDataStyleSelector& other );
    ~GeoDataStyleSelector();
private:
    GeoDataStyleSelectorPrivate * const d;
};

}

#endif
