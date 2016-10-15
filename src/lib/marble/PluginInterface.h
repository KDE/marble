//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Most of the code is taken from MarbleRenderPluginInterface.h
// by Torsten Rahn and Inge Wallin.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//
#ifndef MARBLE_PLUGININTERFACE_H
#define MARBLE_PLUGININTERFACE_H

#include <QVector>
#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS
#include <QString>
#include <QtPlugin>

#include "marble_export.h"

class QIcon;

namespace Marble
{

struct MARBLE_EXPORT PluginAuthor
{
    Q_DECLARE_TR_FUNCTIONS(PluginAuthor)
public:
    PluginAuthor()
    {}

    PluginAuthor( const QString &name_, const QString &email_, const QString &task_ = PluginAuthor::tr( "Developer" ) ) :
        name( name_ ),
        task( task_ ),
        email( email_ )
    {}

    QString name;
    QString task;
    QString email;
};

/**
 * @short This class specifies interface of a Marble plugin.
 */

class MARBLE_EXPORT PluginInterface
{
 public:
    virtual ~PluginInterface();

    /**
     * @brief Returns the user-visible name of the plugin.
     *
     * The user-visible name should be context free, i.e. the name should
     * provide enough information as to what the plugin is about in the context
     * of Marble.
     *
     * Example: "Starry Sky Background", "OpenRouteService Routing"
     */
    virtual QString name() const = 0;

    /**
     * @brief Returns the unique name of the plugin.
     *
     * Examples: "starrysky", "openrouteservice"
     */
    virtual QString nameId() const = 0;

    virtual QString version() const = 0;

    /**
     * @brief Returns a user description of the plugin.
     */
    virtual QString description() const = 0;

    /**
     * @brief Returns an icon for the plugin.
     */
    virtual QIcon icon() const = 0;

    virtual QString copyrightYears() const = 0;

    /**
     * @since 0.26.0
     */
    virtual QVector<PluginAuthor> pluginAuthors() const = 0;

    /**
     * @brief Returns about text (credits) for external data the plugin uses.
     *
     * The default implementation returns the empty string. Please override
     * this method to give credits for all data from 3rd-partys.
     */
    virtual QString aboutDataText() const;
};

}

Q_DECLARE_TYPEINFO(Marble::PluginAuthor, Q_MOVABLE_TYPE);

Q_DECLARE_INTERFACE( Marble::PluginInterface, "org.kde.Marble.PluginInterface/1.1" )

#endif
