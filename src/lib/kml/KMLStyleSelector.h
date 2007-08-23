//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLSTYLESELECTOR_H
#define KMLSTYLESELECTOR_H

#include <QtCore/QString>
#include "KMLObject.h"

class KMLStyleSelector : public KMLObject
{
  public:
    void setStyleId( QString value );
    QString styleId() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  protected:
    KMLStyleSelector();

  protected:
    QString m_styleId;
};

#endif
