// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATADATAPRIVATE_H
#define GEODATADATAPRIVATE_H

#include <QString>
#include <QVariant>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDataPrivate
{
public:
    QVariant m_value;
    QString m_name;
    QString m_displayName;
};

} // namespace Marble

#endif // GEODATADATAPRIVATE_H
