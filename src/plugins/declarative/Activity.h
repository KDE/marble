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

#include "RelatedActivities.h"

namespace Marble
{
    
namespace Declarative
{

class Activity : public QObject
{

    Q_OBJECT
    
    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( QString imagePath READ imagePath WRITE setImagePath )
    Q_PROPERTY( QString path READ path WRITE setPath )
    Q_PROPERTY( QStringList enablePlugins READ enablePlugins WRITE setEnablePlugins )
    Q_PROPERTY( QStringList disablePlugins READ disablePlugins WRITE setDisablePlugins )
    Q_PROPERTY( QVariant relatedActivities READ relatedActivities WRITE setRelatedActivities )
    Q_PROPERTY( QVariant settings READ settings WRITE setSettings )
    
 public:
    explicit Activity();
    explicit Activity( const QString& name, const QString& imagePath, const QString& path,
                       const QStringList& enablePlugins, const QStringList& disablePlugins,
                       const QMap<QString, QVariant>& relatedActivities,
                       const QMap<QString, QVariant>& settings
                     );
    
 public Q_SLOTS:
    void setName( const QString& name );
    QString name() const;
    
    void setImagePath( const QString& imagePath );
    QString imagePath() const;
    
    void setPath( const QString& path );
    QString path() const;
    
    void setEnablePlugins( const QStringList& enablePlugins );
    QStringList enablePlugins() const;
    
    void setDisablePlugins( const QStringList& disablePlugins );
    QStringList disablePlugins() const;
    
    void setRelatedActivities( const QVariant& relatedActivities );
    QMap<QString, QVariant> relatedActivities() const;
    
    void setSettings( const QVariant& settings );
    QMap<QString, QVariant> settings() const;

 private:
    Q_DISABLE_COPY( Activity )
    QString m_name;
    QString m_imagePath;
    QString m_path;
    QStringList m_enablePlugins;
    QStringList m_disablePlugins;
    RelatedActivities m_relatedActivities;
    QMap<QString, QVariant> m_settings;
};

}

}

#endif
