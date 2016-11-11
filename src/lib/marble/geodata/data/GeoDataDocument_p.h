//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATADOCUMENTPRIVATE_H
#define MARBLE_GEODATADOCUMENTPRIVATE_H

#include "GeoDataStyle.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataStyleMap.h"
#include "GeoDataSchema.h"
#include "GeoDataContainer_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDocumentPrivate : public GeoDataContainerPrivate
{
  public:
    GeoDataDocumentPrivate()
    :  m_documentRole( UnknownDocument )
    {
    }

    GeoDataDocumentPrivate(const GeoDataDocumentPrivate& other)
      : GeoDataContainerPrivate(other),
        m_styleHash(other.m_styleHash),
        m_styleMapHash(other.m_styleMapHash),
        m_schemaHash(other.m_schemaHash),
        m_filename(other.m_filename),
        m_baseUri(other.m_baseUri),
        m_networkLinkControl(other.m_networkLinkControl),
        m_property(other.m_property),
        m_documentRole(other.m_documentRole)
    {
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataDocumentId;
    }

    QMap<QString, GeoDataStyle::Ptr> m_styleHash;
    QMap<QString, GeoDataStyleMap> m_styleMapHash;
    QMap<QString, GeoDataSchema> m_schemaHash;
    QString m_filename;
    QString m_baseUri;
    GeoDataNetworkLinkControl m_networkLinkControl;
    QString m_property;
    DocumentRole m_documentRole;
};

} // namespace Marble

#endif
