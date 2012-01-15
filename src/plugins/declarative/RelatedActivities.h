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

/**
 * Stores related activities and plugin states for an activity.
 */
class RelatedActivities : public QObject
{

    Q_OBJECT
    
 public:
    explicit RelatedActivities();
    
    /**
     * @param relatedActivities Map with activity names as key
     * and lists of plugin names as values.
     */
    explicit RelatedActivities( const QMap<QString, QVariant>& relatedActivities );
    
 public Q_SLOTS:
    /**
     * Returns a list of plugins for the related activity with the passed name.
     * 
     * @param name Name of the related activity.
     */
    QStringList get( const QString& name ) const;
    
    /**
     * @param name relatedActivities Map with activity names as key
     * and lists of plugin names as values.
     */
    void setRelatedActivities( const QMap<QString, QVariant>& relatedActivities );
    
    /**
     * @return Map with activity names as key
     * and lists of plugin names as values.
     */
    QMap<QString, QVariant> relatedActivities() const;

 private:
    Q_DISABLE_COPY( RelatedActivities )
    QMap<QString, QVariant> m_relatedActivities;   ///< Map with activity names as key and lists of plugin names as values.
};

#endif
