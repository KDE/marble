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


#ifndef GEODATASTYLESELECTOR_H
#define GEODATASTYLESELECTOR_H


#include <QtCore/QString>

#include "GeoDataObject.h"

#include "geodata_export.h"

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
     * @brief Serialize the styleselector to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the styleselector from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  protected:
    GeoDataStyleSelector();
    ~GeoDataStyleSelector();

  private:
    Q_DISABLE_COPY( GeoDataStyleSelector )
    GeoDataStyleSelectorPrivate * const d;
};

#endif // GEODATASTYLESELECTOR_H
