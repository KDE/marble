//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESPLUGIN_H
#define MARBLE_ECLIPSESPLUGIN_H

#include "RenderPlugin.h"

#include <QtCore/QObject>

namespace Marble
{

/**
 * @brief This plugin displays solar eclipses.
 *
 */
class EclipsesPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( EclipsesPlugin )

public:
    EclipsesPlugin();
    explicit EclipsesPlugin( const MarbleModel *marbleModel );
    virtual ~EclipsesPlugin();
    
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString nameId() const;
    QString guiString() const;
    QString version() const;
    QString description() const;
    QString copyrightYears() const;
    QList<PluginAuthor> pluginAuthors() const;
    QIcon icon() const;
    RenderType renderType() const;

    void initialize();
    bool isInitialized() const;

    bool render( GeoPainter *painter,
                 ViewportParams *viewport,
                 const QString &renderPos,
                 GeoSceneLayer *layer );

    QHash<QString, QVariant> settings() const;
    void setSettings( const QHash<QString, QVariant> &settings );

private Q_SLOTS:
    void enableModel( bool enabled );
    void visibleModel( bool visible );
    void readSettings();
    void writeSettings();
    void updateSettings();

private:
    bool m_isInitialized;
    QHash<QString, QVariant> m_settings;
};

}

#endif // MARBLE_ECLIPSESPLUGIN_H
