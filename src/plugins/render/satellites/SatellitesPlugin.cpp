//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "PluginAboutDialog.h"
#include "GeoDataPlacemark.h"
#include "SatellitesItem.h"
#include "ViewportParams.h"

#include "sgp4/sgp4io.h"

#include "ui_SatellitesConfigDialog.h"

#include <QtCore/QUrl>
#include <QtGui/QPushButton>

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : RenderPlugin(),
     m_model( 0 ),
     m_isInitialized( false ),
     m_aboutDialog( 0 ),
     m_configDialog( 0 ),
     ui_configWidget( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)), SLOT(updateSettings()) );

    setSettings( QHash<QString, QVariant>() );
}

SatellitesPlugin::~SatellitesPlugin()
{
    delete m_model;

    delete m_aboutDialog;
    delete m_configDialog;
    delete ui_configWidget;
}

QStringList SatellitesPlugin::backendTypes() const
{
    return QStringList( "satellites" );
}

QString SatellitesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList SatellitesPlugin::renderPosition() const
{
    return QStringList( "ORBIT" );
}

QString SatellitesPlugin::name() const
{
    return tr( "Satellites" );
}

QString SatellitesPlugin::nameId() const
{
    return "satellites";
}

QString SatellitesPlugin::guiString() const
{
    return tr( "&Satellites" );
}

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays satellites and their orbits." );
}

QIcon SatellitesPlugin::icon() const
{
    return QIcon();
}

void SatellitesPlugin::initialize()
{
    //FIXME: remove the const_cast, it may be best to create a new type of plugins where
    //marbleModel() is not const, since traditional RenderPlugins do not require that
    m_model = new SatellitesModel( const_cast<MarbleModel *>(marbleModel())->treeModel(), marbleModel()->pluginManager() );
    m_isInitialized = true;
    updateSettings();
}

bool SatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool SatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    return true;
}

QHash<QString, QVariant> SatellitesPlugin::settings() const
{
    return m_settings;
}

void SatellitesPlugin::setSettings( QHash<QString, QVariant> settings )
{
    if ( !settings.contains( "tleList" ) ) {
        QStringList tleList;
        tleList << "visual.txt";
        settings.insert( "tleList", tleList );
    } else if ( settings.value( "tleList" ).type() == QVariant::String ) {
        //HACK: KConfig can't guess the type of the settings, when we use KConfigGroup::readEntry()
        // in marble_part it returns a QString which is then wrapped into a QVariant when added
        // to the settings hash. QVariant can handle the conversion for some types, like toDateTime()
        // but when calling toStringList() on a QVariant::String, it will return a one element list
        settings.insert( "tleList", settings.value( "tleList" ).toString().split( "," ) );
    }

    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
}

void SatellitesPlugin::readSettings()
{
    if ( !m_configDialog )
        return;

    QStringList tleList = m_settings.value( "tleList" ).toStringList();
    foreach (const QString &tle, tleList) {
        mDebug() << "Checking " << tle;
        m_boxHash[tle]->setChecked( true );
    }
}

void SatellitesPlugin::writeSettings()
{
    QStringList tleList;
    QHash<QString, QCheckBox*>::const_iterator it = m_boxHash.constBegin();
    QHash<QString, QCheckBox*>::const_iterator const end = m_boxHash.constEnd();
    for ( ; it != end; ++it ) {
        if ( it.value()->isChecked() ) {
            tleList << it.key();
        }
    }

    m_settings.insert( "tleList", tleList );

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }
    foreach ( const QString &tle, m_settings["tleList"].toStringList() ) {
        mDebug() << tle;
        m_model->downloadFile( QUrl( "http://www.celestrak.com/NORAD/elements/" + tle ), tle );
    }
}

QDialog *SatellitesPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Satellites Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        m_aboutDialog->setAboutText( tr( "<br />(c) 2011 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author gmartres;
        gmartres.name = "Guillaume Martres";
        gmartres.task = tr( "Developer" );
        gmartres.email = "smarter@ubuntu.com";
        authors.append( gmartres );
        m_aboutDialog->setAuthors( authors );
        m_aboutDialog->setDataText( tr( "Satellites orbital elements from <a href=\"http://www.celestrak.com\">http://www.celestrak.com</a>" ) );
        m_aboutDialog->setPixmap( icon().pixmap( 62, 53 ) );
    }
    return m_aboutDialog;
}

QDialog *SatellitesPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::SatellitesConfigDialog();
        ui_configWidget->setupUi( m_configDialog );
        populateBoxHash();
        readSettings();

        connect( m_configDialog, SIGNAL(accepted()), SLOT(writeSettings()) );
        connect( m_configDialog, SIGNAL(rejected()), SLOT(readSettings()) );
        connect( ui_configWidget->buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
    }

    return m_configDialog;
}

void SatellitesPlugin::populateBoxHash()
{
    m_boxHash["visual.txt"] = ui_configWidget->visualBox;
    m_boxHash["stations.txt"] = ui_configWidget->stationsBox;
    m_boxHash["tle-new.txt"] = ui_configWidget->tle_newBox;
    m_boxHash["weather.txt"] = ui_configWidget->weatherBox;
    m_boxHash["noaa.txt"] = ui_configWidget->noaaBox;
    m_boxHash["goes.txt"] = ui_configWidget->goesBox;
}

}

Q_EXPORT_PLUGIN2( SatellitesPlugin, Marble::SatellitesPlugin )

#include "SatellitesPlugin.moc"
