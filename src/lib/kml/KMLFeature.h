//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLFEATURE_H
#define KMLFEATURE_H

#include "KMLObject.h"
#include "placemark.h"

#include <QtCore/QString>

class KMLFeature
  : public KMLObject,
    public PlaceMark
{
 public:
    virtual ~KMLFeature();

    QString name() const;
    void setName( const QString &value );

    QString address() const;
    void setAddress( const QString &value);

    QString phoneNumber() const;
    void setPhoneNumber( const QString &value);

    QString description() const;
    void setDescription( const QString &value);

 protected:
    KMLFeature();

 private:
    QString m_name;
    QString m_address;
    QString m_phoneNumber;
    QString m_description;
};

#endif // KMLFEATURE_H
