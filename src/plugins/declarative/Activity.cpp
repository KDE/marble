//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#include "Activity.h"

namespace Marble
{
    
namespace Declarative
{

Activity::Activity()
{
}

Activity::Activity( const QString& name, const QString& imagePath, 
                    const QStringList& enablePlugins, const QStringList& disablePlugins,
                    const QMap<QString, QVariant>& relatedActivities ) : m_name ( name ),
                                                                         m_imagePath( imagePath ),
                                                                         m_enablePlugins( enablePlugins ),
                                                                         m_disablePlugins( disablePlugins ),
                                                                         m_relatedActivities( relatedActivities )
{
}

void Activity::setName( const QString& name )
{
    m_name = name;
}

QString Activity::name()
{
    return m_name;
}

void Activity::setImagePath( const QString& imagePath )
{
    m_imagePath = imagePath;
}

QString Activity::imagePath()
{
    return m_imagePath;
}

void Activity::setEnablePlugins( const QStringList& enablePlugins )
{
    m_enablePlugins = enablePlugins;
}

QStringList Activity::enablePlugins() const
{
    return m_enablePlugins;
}

void Activity::setDisablePlugins( const QStringList& disablePlugins )
{
    m_disablePlugins = disablePlugins;
}

QStringList Activity::disablePlugins() const
{
    return m_disablePlugins;
}

void Activity::setRelatedActivities( const QVariant& relatedActivities )
{
    m_relatedActivities.setRelatedActivities( relatedActivities.toMap() );
}

QMap<QString, QVariant> Activity::relatedActivities() const
{
    return m_relatedActivities.relatedActivities();
}

}

}

#include "Activity.moc"