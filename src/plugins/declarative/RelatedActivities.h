//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#ifndef MARBLE_RELATEDACTIVITIES_H
#define MARBLE_RELATEDACTIVITIES_H

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

namespace Marble
{
    
namespace Declarative
{

class RelatedActivities : public QObject
{

    Q_OBJECT
    
 public:
    explicit RelatedActivities();
    explicit RelatedActivities( const QMap<QString, QVariant>& relatedActivities );
    
 public Q_SLOTS:
    QStringList get( const QString& name ) const;
    void setRelatedActivities( const QMap<QString, QVariant>& relatedActivities );
    QMap<QString, QVariant> relatedActivities() const;

 private:
    Q_DISABLE_COPY( RelatedActivities )
    QMap<QString, QVariant> m_relatedActivities;
};

}

}

#endif
