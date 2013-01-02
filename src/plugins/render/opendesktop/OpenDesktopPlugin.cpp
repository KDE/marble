//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#include "OpenDesktopPlugin.h"
#include "OpenDesktopModel.h"

#include "ui_OpenDesktopConfigWidget.h"

#include <QtGui/QPushButton>

using namespace Marble;

OpenDesktopPlugin::OpenDesktopPlugin()
    : AbstractDataPlugin( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
}

OpenDesktopPlugin::OpenDesktopPlugin( const MarbleModel *marbleModel )
    : AbstractDataPlugin( marbleModel ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
}

void OpenDesktopPlugin::initialize()
{
    setModel( new OpenDesktopModel( pluginManager(), this ) );
    setNumberOfItems( defaultItemsOnScreen ); // Setting the number of items on the screen.
}

QString OpenDesktopPlugin::name() const
{
    return tr( "OpenDesktop Items" );
}

QString OpenDesktopPlugin::guiString() const
{
    return tr( "&OpenDesktop Community" );
}

QString OpenDesktopPlugin::nameId() const
{
    return "opendesktop";
}

QString OpenDesktopPlugin::version() const
{
    return "1.0";
}

QString OpenDesktopPlugin::description() const
{
    return tr( "Shows OpenDesktop users' avatars and some extra information about them on the map." );
}

QString OpenDesktopPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> OpenDesktopPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Utku Aydin" ), "utkuaydin34@gmail.com" );
}

QIcon OpenDesktopPlugin::icon() const
{
    return QIcon(":/icons/social.png");
}

QDialog *OpenDesktopPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        m_uiConfigWidget = new Ui::OpenDesktopConfigWidget;
        m_uiConfigWidget->setupUi( m_configDialog );
        readSettings();

        connect( m_uiConfigWidget->m_buttonBox, SIGNAL(accepted()),
                SLOT(writeSettings()) );
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL(rejected()),
                SLOT(readSettings()) );
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button( 
                                                         QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 this,        SLOT(writeSettings()) );
    }

    return m_configDialog;
}

QHash<QString,QVariant> OpenDesktopPlugin::settings() const
{
    QHash<QString, QVariant> settings;

    settings.insert( "itemsOnScreen", numberOfItems() );

    return settings;
}

void OpenDesktopPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    setNumberOfItems( settings.value( "itemsOnScreen", defaultItemsOnScreen ).toInt() );

    emit settingsChanged( nameId() );
}

void OpenDesktopPlugin::readSettings()
{
    if ( m_uiConfigWidget ) {
        m_uiConfigWidget->m_itemsOnScreenSpin->setValue( numberOfItems() );
    }
}

void OpenDesktopPlugin::writeSettings()
{
    if ( m_uiConfigWidget ) {
        setNumberOfItems( m_uiConfigWidget->m_itemsOnScreenSpin->value() );
    }

    emit settingsChanged( nameId() );
}

// Because we want to create a plugin, we have to do the following line.
Q_EXPORT_PLUGIN2( OpenDesktopPlugin, Marble::OpenDesktopPlugin )

#include "OpenDesktopPlugin.moc"
