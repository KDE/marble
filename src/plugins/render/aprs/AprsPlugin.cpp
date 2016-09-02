//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "AprsPlugin.h"

#include "MarbleGlobal.h"
#include "MarbleDebug.h"

#include <QColor>
#include <QAction>
#include <QMutexLocker>
#include <QTcpSocket>

#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"
#include "MarbleModel.h"
#include "GeoDataLatLonAltBox.h"
#include "ViewportParams.h"
#include "AprsGatherer.h"
#include "AprsTCPIP.h"
#include "AprsFile.h"

#include <aprsconfig.h>

#ifdef HAVE_QTSERIALPORT
#include "AprsTTY.h"
#endif

using namespace Marble;
/* TRANSLATOR Marble::AprsPlugin */

AprsPlugin::AprsPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_mutex( new QMutex ),
      m_initialized( false ),
      m_tcpipGatherer( 0 ),
      m_ttyGatherer( 0 ),
      m_fileGatherer( 0 ),
      m_action( 0 ),
      m_useInternet( true ),
      m_useTty( false ),
      m_useFile( false ),
      m_aprsHost( "rotate.aprs.net" ),
      m_aprsPort( 10253 ),
      m_tncTty( "/dev/ttyUSB0" ),
      m_aprsFile(),
      m_dumpTcpIp( false ),
      m_dumpTty( false ),
      m_dumpFile( false ),
      m_fadeTime( 10 ),
      m_hideTime( 45 ),
      m_configDialog( 0 ),
      ui_configWidget( 0 )
{
    setEnabled( true );
    setVisible( false );
    
    setSettings( QHash<QString,QVariant>() );

    connect( this, SIGNAL(visibilityChanged(bool,QString)),
             this, SLOT(updateVisibility(bool)) );

    m_action = new QAction( this );
    connect( m_action,    SIGNAL(toggled(bool)),
	     this,        SLOT(setVisible(bool)) );

}

AprsPlugin::~AprsPlugin()
{
    stopGatherers();

    delete m_configDialog;
    delete ui_configWidget;

    QMap<QString, AprsObject *>::Iterator obj;
    QMap<QString, AprsObject *>::Iterator end = m_objects.end();

    for( obj = m_objects.begin(); obj != end; ++obj ) {
        delete *obj;
    }

    m_objects.clear();

    delete m_mutex;
}

void AprsPlugin::updateVisibility( bool visible )
{
    if ( visible )
        restartGatherers();
    else
        stopGatherers();
}

RenderPlugin::RenderType AprsPlugin::renderType() const
{
    return OnlineRenderType;
}

QStringList AprsPlugin::backendTypes() const
{
    return QStringList(QStringLiteral("aprs"));
}

QString AprsPlugin::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList AprsPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}

QString AprsPlugin::name() const
{
    return tr( "Amateur Radio Aprs Plugin" );
}

QString AprsPlugin::guiString() const
{
    return tr( "Amateur Radio &Aprs Plugin" );
}

QString AprsPlugin::nameId() const
{
    return QStringLiteral("aprs-plugin");
}

QString AprsPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString AprsPlugin::description() const
{
    return tr( "This plugin displays APRS data gleaned from the Internet.  APRS is an Amateur Radio protocol for broadcasting location and other information." );
}

QString AprsPlugin::copyrightYears() const
{
    return QStringLiteral("2009, 2010");
}

QVector<PluginAuthor> AprsPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Wes Hardaker"), QStringLiteral("hardaker@users.sourceforge.net"));
}

QIcon AprsPlugin::icon () const
{
    return QIcon(QStringLiteral(":/icons/aprs.png"));
}

void AprsPlugin::stopGatherers()
{

    // tell them all to stop
    if ( m_tcpipGatherer )
        m_tcpipGatherer->shutDown();

#ifdef HAVE_QTSERIALPORT
    if ( m_ttyGatherer )
        m_ttyGatherer->shutDown();
#endif
    
    if ( m_fileGatherer )
        m_fileGatherer->shutDown();
    
    // now wait for them for at least 2 seconds (it shouldn't take that long)
    if ( m_tcpipGatherer )
        if ( m_tcpipGatherer->wait(2000) )
            delete m_tcpipGatherer;

#ifdef HAVE_QTSERIALPORT
    if ( m_ttyGatherer )
        if ( m_ttyGatherer->wait(2000) )
            delete m_ttyGatherer;
#endif
    
    if ( m_fileGatherer )
        if ( m_fileGatherer->wait(2000) )
            delete m_fileGatherer;

    m_tcpipGatherer = 0;
    m_ttyGatherer = 0;
    m_fileGatherer = 0;
}

void AprsPlugin::restartGatherers()
{
    stopGatherers();

    if ( m_useInternet ) {
        m_tcpipGatherer =
            new AprsGatherer( new AprsTCPIP( m_aprsHost, m_aprsPort ),
                              &m_objects, m_mutex, &m_filter);
        m_tcpipGatherer->setSeenFrom( GeoAprsCoordinates::FromTCPIP );
        m_tcpipGatherer->setDumpOutput( m_dumpTcpIp );

        m_tcpipGatherer->start();
        mDebug() << "started TCPIP gatherer";
    }

#ifdef HAVE_QTSERIALPORT
    if ( m_useTty ) {
        m_ttyGatherer =
            new AprsGatherer( new AprsTTY( m_tncTty ),
                              &m_objects, m_mutex, NULL);

        m_ttyGatherer->setSeenFrom( GeoAprsCoordinates::FromTTY );
        m_ttyGatherer->setDumpOutput( m_dumpTty );

        m_ttyGatherer->start();
        mDebug() << "started TTY gatherer";
    }
#endif

    
    if ( m_useFile ) {
        m_fileGatherer = 
            new AprsGatherer( new AprsFile( m_aprsFile ),
                              &m_objects, m_mutex, NULL);

        m_fileGatherer->setSeenFrom( GeoAprsCoordinates::FromFile );
        m_fileGatherer->setDumpOutput( m_dumpFile );

        m_fileGatherer->start();
        mDebug() << "started File gatherer";
    }
}


void AprsPlugin::initialize ()
{
    m_initialized = true;
    mDebug() << "APRS initialized";

    restartGatherers();
}

QDialog *AprsPlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::AprsConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        readSettings();
        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()),
                 SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox, SIGNAL(rejected()),
                 SLOT(readSettings()) );
        //       QPushButton *applyButton =
//             ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
//         connect( applyButton, SIGNAL(clicked()),
//                  this,        SLOT(writeSettings()) );
    }
    return m_configDialog;
}

void AprsPlugin::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

#ifndef HAVE_QTSERIALPORT
    ui_configWidget->tabWidget->setTabEnabled( ui_configWidget->tabWidget->indexOf(
                                                   ui_configWidget->Device ), false );
#endif

    // Connect to the net?
    if ( m_useInternet )
        ui_configWidget->m_internetBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_internetBox->setCheckState( Qt::Unchecked );

    // Connection Information
    ui_configWidget->m_serverName->setText( m_aprsHost );
    ui_configWidget->m_serverPort->setText( QString::number( m_aprsPort ) );

    // Read from a TTY serial port?
    if ( m_useTty )
        ui_configWidget->m_serialBox->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_serialBox->setCheckState( Qt::Unchecked );

    // Serial port to use
    ui_configWidget->m_ttyName->setText( m_tncTty );

    // Read from a File?
    if ( m_useFile )
        ui_configWidget->m_useFile->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_useFile->setCheckState( Qt::Unchecked );

    // Serial port to use
    ui_configWidget->m_fileName->setText( m_aprsFile );

    // Dumping settings
    if ( m_dumpTcpIp )
        ui_configWidget->m_tcpipdump->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_tcpipdump->setCheckState( Qt::Unchecked );

    if ( m_dumpTty )
        ui_configWidget->m_ttydump->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_ttydump->setCheckState( Qt::Unchecked );

    if ( m_dumpFile )
        ui_configWidget->m_filedump->setCheckState( Qt::Checked );
    else
        ui_configWidget->m_filedump->setCheckState( Qt::Unchecked );

    // display settings
    ui_configWidget->m_fadetime->setText( QString::number( m_fadeTime ) );
    ui_configWidget->m_hidetime->setText( QString::number( m_hideTime ) );
}


void AprsPlugin::writeSettings()
{
    m_useInternet = ui_configWidget->m_internetBox->checkState() == Qt::Checked;
    m_useTty = ui_configWidget->m_serialBox->checkState() == Qt::Checked;
    m_useFile = ui_configWidget->m_useFile->checkState() == Qt::Checked;

    m_aprsHost = ui_configWidget->m_serverName->text();
    m_aprsPort = ui_configWidget->m_serverPort->text().toInt();
    m_tncTty = ui_configWidget->m_ttyName->text();

    m_dumpTcpIp = ui_configWidget->m_tcpipdump->checkState() == Qt::Checked;
    m_dumpTty = ui_configWidget->m_ttydump->checkState() == Qt::Checked;
    m_dumpFile = ui_configWidget->m_filedump->checkState() == Qt::Checked;

    m_fadeTime = ui_configWidget->m_fadetime->text().toInt();
    m_hideTime = ui_configWidget->m_hidetime->text().toInt();

    restartGatherers();
    emit settingsChanged( nameId() );
}

QHash<QString,QVariant> AprsPlugin::settings() const
{
    QHash<QString, QVariant> result = RenderPlugin::settings();

    result.insert(QStringLiteral("useInternet"), m_useInternet);
    result.insert(QStringLiteral("useTTY"), m_useTty);
    result.insert(QStringLiteral("useFile"), m_useFile);
    result.insert(QStringLiteral("APRSHost"), m_aprsHost);
    result.insert(QStringLiteral("APRSPort"), m_aprsPort);
    result.insert(QStringLiteral("TNCTTY"), m_tncTty);
    result.insert(QStringLiteral("FileName"), m_aprsFile);
    result.insert(QStringLiteral("TCPIPDump"), m_dumpTcpIp);
    result.insert(QStringLiteral("TTYDump"), m_dumpTty);
    result.insert(QStringLiteral("FileDump"), m_dumpFile);
    result.insert(QStringLiteral("fadeTime"), m_fadeTime);
    result.insert(QStringLiteral("hideTime"), m_hideTime);

    return result;
}

void AprsPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    m_useInternet =  settings.value(QStringLiteral("useInternet"), true).toBool();
    m_useTty = settings.value(QStringLiteral("useTTY"), false).toBool();
    m_useFile = settings.value(QStringLiteral("useFile"), false).toBool();

    m_aprsHost = settings.value(QStringLiteral("APRSHost"), QStringLiteral("rotate.aprs.net")).toString();
    m_aprsPort = settings.value(QStringLiteral("APRSPort"), 10253).toInt();
    m_tncTty = settings.value(QStringLiteral("TNCTTY"), QStringLiteral("/dev/ttyUSB0")).toString();
    m_aprsFile = settings.value(QStringLiteral("FileName"), QString()).toString();

    m_dumpTcpIp = settings.value(QStringLiteral("TCPIPDump"), false).toBool();
    m_dumpTty = settings.value(QStringLiteral("TTYDump"), false).toBool();
    m_dumpFile = settings.value(QStringLiteral("FileDump"), false).toBool();

    m_fadeTime = settings.value(QStringLiteral("fadeTime"), 10).toInt();
    m_hideTime = settings.value(QStringLiteral("hideTime"), 45).toInt();

    readSettings();
    emit settingsChanged( nameId() );
}

bool AprsPlugin::isInitialized () const
{
    return m_initialized;
}

bool AprsPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    int fadetime = m_fadeTime * 60000;
    int hidetime = m_hideTime * 60000;

    painter->save();

    if ( !( viewport->viewLatLonAltBox() == m_lastBox ) ) {
        m_lastBox = viewport->viewLatLonAltBox();
        QString towrite = QLatin1String("#filter a/") +
            QString::number(m_lastBox.north(GeoDataCoordinates::Degree)) + QLatin1Char('/') +
            QString::number(m_lastBox.west(GeoDataCoordinates::Degree))  + QLatin1Char('/') +
            QString::number(m_lastBox.south(GeoDataCoordinates::Degree)) + QLatin1Char('/') +
            QString::number(m_lastBox.east(GeoDataCoordinates::Degree))  + QLatin1Char('\n');
        mDebug() << "upating filter: " << towrite.toLocal8Bit().data();

        QMutexLocker locker( m_mutex );
        m_filter = towrite;
    }
    

    QMutexLocker locker( m_mutex );
    QMap<QString, AprsObject *>::ConstIterator obj;
    for( obj = m_objects.constBegin(); obj != m_objects.constEnd(); ++obj ) {
        ( *obj )->render( painter, viewport, fadetime, hidetime );
    }

    painter->restore();

    return true;
}

QAction* AprsPlugin::action() const
{
    m_action->setCheckable( true );
    m_action->setChecked( visible() );
    m_action->setIcon( icon() );
    m_action->setText( guiString() );
    m_action->setToolTip( description() );
    return m_action;
}

#include "moc_AprsPlugin.cpp"
