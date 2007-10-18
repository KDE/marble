//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATASTYLESELECTOR_H
#define GEODATASTYLESELECTOR_H

#include <QtCore/QString>
#include "GeoDataObject.h"

class GeoDataStyleSelector : public GeoDataObject
{
  public:
    void setStyleId( const QString &value );
    QString styleId() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  protected:
    GeoDataStyleSelector();

  protected:
    QString m_styleId;
};

#endif // GEODATASTYLESELECTOR_H
