//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATAFEATURE_H
#define GEODATAFEATURE_H


#include <QtCore/QString>
#include "GeoDataObject.h"

class GeoDataFeature
  : public GeoDataObject
{
 public:
    virtual ~GeoDataFeature();

    QString name() const;
    void setName( const QString &value );

    QString address() const;
    void setAddress( const QString &value);

    QString phoneNumber() const;
    void setPhoneNumber( const QString &value);

    QString description() const;
    void setDescription( const QString &value);

    bool isVisible() const;
    void setVisible( bool value );

    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataFeature();
    GeoDataFeature( const QString& name );

 private:
    QString     m_name;
    QString     m_address;
    QString     m_phoneNumber;
    QString     m_description;
    bool        m_visible;
};

#endif // GEODATAFEATURE_H
