//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#include "RelatedActivities.h"

namespace Marble
{
    
namespace Declarative
{

RelatedActivities::RelatedActivities()
{
}

RelatedActivities::RelatedActivities( const QMap<QString, QVariant>& relatedActivities ) : m_relatedActivities( relatedActivities )
{
}

QStringList RelatedActivities::get( const QString& name ) const
{
    return m_relatedActivities[name].toStringList();
}

void RelatedActivities::setRelatedActivities( const QMap<QString, QVariant>& relatedActivities )
{
    m_relatedActivities = relatedActivities;
}

QMap<QString, QVariant> RelatedActivities::relatedActivities() const
{
    return m_relatedActivities;
}

}

}

#include "RelatedActivities.moc"