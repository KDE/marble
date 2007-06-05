#ifndef KMLFEATURE_H
#define KMLFEATURE_H

#include "KMLObject.h"

#include <QtCore/QString>

class KMLFeature : public KMLObject
{
 public:
    QString name() const;
    void setName( QString &value );

    QString address() const;
    void setAddress( QString &value);

    QString phoneNumber() const;
    void setPhoneNumber( QString &value);

    QString description() const;
    void setDescription( QString &value);

 protected:
    KMLFeature();

 private:
    QString m_name;
    QString m_address;
    QString m_phoneNumber;
    QString m_description;
};

#endif // KMLFEATURE_H
