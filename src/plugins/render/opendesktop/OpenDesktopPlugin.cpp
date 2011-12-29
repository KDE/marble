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
    : m_isInitialized(false),
      m_itemsOnScreen( defaultItemsOnScreen ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
    setNameId( "opendesktop" );
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default

    setVersion( "0.1" );
    setCopyrightYear( 2010 );
    addAuthor( QString::fromUtf8( "Utku Aydin" ), "utkuaydin34@gmail.com" );
}

void OpenDesktopPlugin::initialize()
{
    readSettings();
    setModel( new OpenDesktopModel( pluginManager(), this ) );
    setNumberOfItems( m_itemsOnScreen ); // Setting the number of items on the screen.
    m_isInitialized = true;
}

bool OpenDesktopPlugin::isInitialized() const
{
    return m_isInitialized;
}

QString OpenDesktopPlugin::name() const
{
    return tr( "OpenDesktop Items" );
}

QString OpenDesktopPlugin::guiString() const
{
    return tr( "&OpenDesktop Community" );
}

QString OpenDesktopPlugin::description() const
{
    return tr( "Shows OpenDesktop users' avatars and some extra information about them on the map." );
}

QIcon OpenDesktopPlugin::icon() const
{
    return QIcon();
}

QDialog *OpenDesktopPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        m_uiConfigWidget = new Ui::OpenDesktopConfigWidget;
        m_uiConfigWidget->setupUi( m_configDialog );
        readSettings();

        connect( m_uiConfigWidget->m_buttonBox, SIGNAL( accepted() ),
                SLOT( writeSettings() ) );
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL( rejected() ),
                SLOT( readSettings() ) );
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button( 
                                                         QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 this,        SLOT( writeSettings() ) );
    }

    return m_configDialog;
}

QHash<QString,QVariant> OpenDesktopPlugin::settings() const
{
    return m_settings;
}

void OpenDesktopPlugin::setSettings( QHash<QString,QVariant> settings )
{
    m_settings = settings;
    readSettings();
}

void OpenDesktopPlugin::readSettings()
{
    m_itemsOnScreen = m_settings.value( "itemsOnScreen", defaultItemsOnScreen ).toInt();
    if ( m_uiConfigWidget ) {
        m_uiConfigWidget->m_itemsOnScreenSpin->setValue( m_itemsOnScreen );
    }

    setNumberOfItems( m_itemsOnScreen );
}

void OpenDesktopPlugin::writeSettings()
{
    if ( m_uiConfigWidget ) {
        m_settings["itemsOnScreen"] = m_uiConfigWidget->m_itemsOnScreenSpin->value();
    }

    readSettings();
    emit settingsChanged( nameId() );
}

// Because we want to create a plugin, we have to do the following line.
Q_EXPORT_PLUGIN2( OpenDesktopPlugin, Marble::OpenDesktopPlugin )

#include "OpenDesktopPlugin.moc"
