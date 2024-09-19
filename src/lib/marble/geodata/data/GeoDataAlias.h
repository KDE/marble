// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_GEODATAALIAS_H
#define MARBLE_GEODATAALIAS_H

#include "GeoDocument.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataAliasPrivate;

/**
 */
class GEODATA_EXPORT GeoDataAlias : public GeoNode
{
public:
    GeoDataAlias();

    GeoDataAlias(const GeoDataAlias &other);

    GeoDataAlias &operator=(const GeoDataAlias &other);

    bool operator==(const GeoDataAlias &other) const;

    bool operator!=(const GeoDataAlias &other) const;

    ~GeoDataAlias() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    QString sourceHref() const;

    void setSourceHref(const QString &sourceHref);

    QString targetHref() const;

    void setTargetHref(const QString &targetHref);

private:
    GeoDataAliasPrivate *const d;
};

}

#endif
