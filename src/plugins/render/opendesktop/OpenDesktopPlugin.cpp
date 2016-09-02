//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın        <utkuaydin34@gmail.com>
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "OpenDesktopPlugin.h"
#include "OpenDesktopModel.h"
#include "MarbleWidget.h"

#include "ui_OpenDesktopConfigWidget.h"

#include <QPushButton>

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
    setModel( new OpenDesktopModel( marbleModel(), this ) );
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
    return QStringLiteral("opendesktop");
}

QString OpenDesktopPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OpenDesktopPlugin::description() const
{
    return tr( "Shows OpenDesktop users' avatars and some extra information about them on the map." );
}

QString OpenDesktopPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> OpenDesktopPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Utku Aydin"), QStringLiteral("utkuaydin34@gmail.com"));
}

QIcon OpenDesktopPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/social.png"));
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
    QHash<QString, QVariant> settings = AbstractDataPlugin::settings();

    settings.insert(QStringLiteral("itemsOnScreen"), numberOfItems());

    return settings;
}

bool OpenDesktopPlugin::eventFilter(QObject *object, QEvent *event)
{
    if ( isInitialized() ) {
        OpenDesktopModel *odModel = qobject_cast<OpenDesktopModel*>( model() );
        Q_ASSERT(odModel);
        MarbleWidget* widget = qobject_cast<MarbleWidget*>( object );
        if ( widget ) {
            odModel->setMarbleWidget(widget);
        }
    }

    return AbstractDataPlugin::eventFilter( object, event );
}

void OpenDesktopPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    AbstractDataPlugin::setSettings( settings );

    setNumberOfItems(settings.value(QStringLiteral("itemsOnScreen"), defaultItemsOnScreen).toInt());

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

#include "moc_OpenDesktopPlugin.cpp"
