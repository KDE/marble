//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright (C) 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLE_GEOSCENE_LICENSE_H
#define MARBLE_GEOSCENE_LICENSE_H

#include <QtCore/QString>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

class GEODATA_EXPORT GeoSceneLicense : public GeoNode
{
 public:
    QString license() const;
    QString shortLicense() const;

    void setLicense( const QString &license );
    void setShortLicense( const QString &license );

 private:
    QString m_fullLicense;
    QString m_shortLicense;

};

}

#endif
