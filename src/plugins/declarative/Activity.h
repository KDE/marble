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

/**
 * This class represents an activity.
 * 
 * An activity has its own QML user interface, image, plugin states and settings.
 */
class Activity : public QObject
{

    Q_OBJECT
    
    // Define properties for access QML.
    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( QString imagePath READ imagePath WRITE setImagePath )
    Q_PROPERTY( QString path READ path WRITE setPath )
    Q_PROPERTY( QStringList enablePlugins READ enablePlugins WRITE setEnablePlugins )
    Q_PROPERTY( QStringList disablePlugins READ disablePlugins WRITE setDisablePlugins )
    Q_PROPERTY( QVariant relatedActivities READ relatedActivities WRITE setRelatedActivities )
    Q_PROPERTY( QVariant settings READ settings WRITE setSettings )
    
 public:
    /**
     * Empty default constructor for model.
     */
    explicit Activity();

    /**
     * Creates a new activity from passed parameters.
     *
     * @param name Name of the activity
     * @param imagePath Path to the image of the activity.
     * @param path Path to the QML file of the activity.
     * @param enablePlugins Names of plugins to enable for this activity.
     * @param disablePlugins Names of plugins to disable for this activity.
     * @param relatedActivities Related activities with plugin states to preserve.
     * @param settings Settings to apply for this activity.
     */
    explicit Activity( const QString& name, const QString& imagePath, const QString& path,
                       const QStringList& enablePlugins, const QStringList& disablePlugins,
                       const QMap<QString, QVariant>& relatedActivities,
                       const QMap<QString, QVariant>& settings
                     );
    
 public Q_SLOTS:
    /**
     * Sets the name of the activity to the passed string.
     *
     * @param name New name of the activity.
     */
    void setName( const QString& name );
    
    /**
     * Returns the name of the activity.
     *
     * @return Name of the activity.
     */
    QString name() const;
    
    /**
     * Sets the path of the image of the activity to the passed string.
     *
     * @param name New path of the image of the activity.
     */
    void setImagePath( const QString& imagePath );
    
    /**
     * Returns the path of the image for the activity.
     *
     * @return Path of the image for the activity.
     */
    QString imagePath() const;
    
    /**
     * Sets the path of the QML file of the activity to the passed string.
     *
     * @param name New path of the QML file of the activity.
     */
    void setPath( const QString& path );
    
    /**
     * Returns the path of the QML file of the activity.
     *
     * @return Path of the QML file of the activity.
     */
    QString path() const;
    
    /**
     * Sets the names of the plugins to enable for the activity.
     *
     * @param name New names of the plugins to enable for the activity.
     */
    void setEnablePlugins( const QStringList& enablePlugins );
    
    /**
     * Returns the names of the plugins to enable for the activity.
     *
     * @return Names of the plugins to enable for the activity.
     */
    QStringList enablePlugins() const;
    
    /**
     * Sets the names of the plugins to disable for the activity.
     *
     * @param name New names of the plugins to disable for the activity.
     */
    void setDisablePlugins( const QStringList& disablePlugins );
    
    /**
     * Returns the names of the plugins to disable for the activity.
     *
     * @return Names of the plugins to disable for the activity.
     */
    QStringList disablePlugins() const;
    
    /**
     * Sets the related activities of the activity.
     *
     * @param name New related activities of the activity.
     */
    void setRelatedActivities( const QVariant& relatedActivities );
    
    /**
     * Returns the related activities of the activity.
     *
     * @return Related activities of the activity.
     */
    QMap<QString, QVariant> relatedActivities() const;
    
    /**
     * Sets the settings of the activity.
     *
     * @param name New settings of the activity.
     */
    void setSettings( const QVariant& settings );
    
    /**
     * Returns the settings of the activity.
     *
     * @return Settings of the activity.
     */
    QMap<QString, QVariant> settings() const;

 private:
    Q_DISABLE_COPY( Activity )
    QString m_name;                            ///< Name of the activity.
    QString m_imagePath;                       ///< Path to the image of the activity.
    QString m_path;                            ///< Path to the QML file of the activity.
    QStringList m_enablePlugins;               ///< Names of plugins to enable for the activity.
    QStringList m_disablePlugins;              ///< Names of plugins to disable for the activity.
    RelatedActivities m_relatedActivities;     ///< Related activities of the activity.
    QMap<QString, QVariant> m_settings;        ///< Settings of the activity.
};

}

}

#endif
