//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLOBJECT_H
#define KMLOBJECT_H

#include "Serializable.h"

class KMLObject : public Serializable
{
 public:
    virtual ~KMLObject();

    int id() const;
    void setId( int value );

    int targetId() const;
    void setTargetId( int value );

    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& steam );

 protected:
    KMLObject();

 private:
    int m_id;
    int m_targetId;
};

#endif // KMLOBJECT_H
