//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSPLUGIN_H
#define APRSPLUGIN_H

#include <QDialog>

#include "RenderPlugin.h"
#include "DialogConfigurationInterface.h"
#include "AprsObject.h"
#include "AprsGatherer.h"
#include "GeoDataLatLonAltBox.h"

#include "ui_AprsConfigWidget.h"

class QMutex;

namespace Ui
{
    class AprsConfigWidget;
}

namespace Marble
{

/**
 * \brief This class displays a layer of aprs (which aprs TBD).
 *
 */
    class AprsPlugin : public RenderPlugin, public DialogConfigurationInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.kde.marble.AprsPlugin")
        Q_INTERFACES( Marble::RenderPluginInterface )
        Q_INTERFACES( Marble::DialogConfigurationInterface )
        MARBLE_PLUGIN( AprsPlugin )

            public:
        explicit AprsPlugin( const MarbleModel *marbleModel=0 );
        ~AprsPlugin();
        QStringList backendTypes() const;
        QString renderPolicy() const;
        QStringList renderPosition() const;
        QString name() const;
        QString guiString() const;
        QString nameId() const;

        QString version() const;

        QString description() const;

        QString copyrightYears() const;

        QVector<PluginAuthor> pluginAuthors() const override;

        QIcon icon () const;

        void initialize ();
        bool isInitialized () const;
        bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

        QDialog *configDialog();
        QAction       *action() const;

        QHash<QString,QVariant> settings() const;
        void setSettings( const QHash<QString,QVariant> &settings );

        void stopGatherers();
        void restartGatherers();


        private Q_SLOTS: 
        void readSettings();
        void writeSettings();
        void updateVisibility( bool visible );
        virtual RenderType renderType() const;

      private:

        QMutex                        *m_mutex;
        QMap<QString, AprsObject *>    m_objects;
        bool m_initialized;
        GeoDataLatLonAltBox            m_lastBox;
        AprsGatherer                  *m_tcpipGatherer,
                                      *m_ttyGatherer,
                                      *m_fileGatherer;
        QString                        m_filter;
        QAction                       *m_action;

        bool m_useInternet;
        bool m_useTty;
        bool m_useFile;
        QString m_aprsHost;
        int m_aprsPort;
        QString m_tncTty;
        QString m_aprsFile;
        bool m_dumpTcpIp;
        bool m_dumpTty;
        bool m_dumpFile;
        int m_fadeTime;
        int m_hideTime;

        QDialog               *m_configDialog;
        Ui::AprsConfigWidget  *ui_configWidget;

    };

}

#endif
