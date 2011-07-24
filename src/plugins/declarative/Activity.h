//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#ifndef MARBLE_ACTIVITY_H
#define MARBLE_ACTIVITY_H


#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT Activity
{

    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( QString imagePath READ imagePath WRITE setImagePath )
    Q_PROPERTY( QStringList enablePlugins READ enablePlugins WRITE setEnablePlugins )
    Q_PROPERTY( QStringList disablePlugins READ disablePlugins WRITE setDisablePlugins )
    Q_PROPERTY( QVariant relatedActivities READ relatedActivities WRITE setRelatedActivities )
    
 public:
    explicit Activity( const QString& name, const QString& imagePath, 
                       const QStringList& enablePlugins, const QStringList& disablePlugins,
                       const QMap<QString, QVariant>& relatedActivities );
    
 public Q_SLOTS:
    void setName( const QString& name );
    QString name();
    
    void setImagePath( const QString& imagePath );
    QString imagePath();
    
    void setEnablePlugins( const QStringList& enablePlugins );
    QStringList enablePlugins() const;
    
    void setDisablePlugins( const QStringList& disablePlugins );
    QStringList disablePlugins() const;
    
    void setRelatedActivities( const QMap<QString, QVariant>& relatedActivities );
    QMap<QString, QVariant> relatedActivities() const;

 private:
    Q_DISABLE_COPY( Activity )
    QString m_name;
    QString m_imagePath;
    QStringList m_enablePlugins;
    QStringList m_disablePlugins;
    QMap<QString, QVariant> m_relatedActivities;
};

}

#endif
