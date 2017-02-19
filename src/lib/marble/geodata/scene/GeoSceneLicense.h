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

#include <QString>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

class GEODATA_EXPORT GeoSceneLicense : public GeoNode
{
 public:
    enum Attribution {
        Never,
        OptOut,
        OptIn,
        Always
    };

    GeoSceneLicense();

    QString license() const;
    QString shortLicense() const;
    Attribution attribution() const;

    void setLicense( const QString &license );
    void setShortLicense( const QString &license );
    void setAttribution(Attribution attr );

    const char *nodeType() const override;

 private:
    QString m_fullLicense;
    QString m_shortLicense;
    Attribution m_attribution;

};

}

#endif
