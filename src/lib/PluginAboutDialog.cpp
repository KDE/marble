//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PluginAboutDialog.h"

// Marble
#include "MarbleDirs.h"
#include "ui_MarbleAboutDialog.h"

// Qt
#include <QtCore/QTextStream>

namespace Marble
{

// The index of the "Data" tab.
int dataTabIndex = 2;

class PluginAboutDialogPrivate
{
 public:
    PluginAboutDialogPrivate()
    {
    }
    ~PluginAboutDialogPrivate()
    {
    }

    Ui::MarbleAboutDialog u_dialog;
};

PluginAboutDialog::PluginAboutDialog( QWidget *parent )
    : QDialog( parent ),
      d( new PluginAboutDialogPrivate() )
{
    d->u_dialog.setupUi( this );

    setAboutText( QString() );
    setAuthorsText( QString() );
    setDataText( QString() );
    setLicenseAgreementText( QString() );
}

PluginAboutDialog::~PluginAboutDialog()
{
    delete d;
}

void PluginAboutDialog::setName( const QString& name )
{
    d->u_dialog.m_pMarbleTitleLabel->setText( name );
    setWindowTitle( tr( "About %1" ).arg( name ) );
}

void PluginAboutDialog::setVersion( const QString& version )
{
    d->u_dialog.m_pMarbleVersionLabel->setText( tr( "Version %1" ).arg( version ) );
}

void PluginAboutDialog::setPixmap( const QPixmap& pixmap )
{
    d->u_dialog.m_pMarbleLogoLabel->setPixmap( pixmap );
}

void PluginAboutDialog::setAboutText( const QString& about )
{
    d->u_dialog.m_pMarbleAboutBrowser->setText( about );
}

void PluginAboutDialog::setAuthors( const QList<Author>& authors )
{
    QString string;
    foreach ( const Author& author, authors ) {
        string += author.name;
        string += "\n    ";
        string += author.email;
        string += "\n    ";
        string += author.task;
        string += "\n\n";
    }

    setAuthorsText( string );
}

void PluginAboutDialog::setAuthorsText( const QString& authors )
{
    d->u_dialog.m_pMarbleAuthorsBrowser->setText( authors );
}

void PluginAboutDialog::setDataText( const QString& data )
{
    if ( data.isNull() ) {
        d->u_dialog.tabWidget->removeTab( d->u_dialog.tabWidget->indexOf( d->u_dialog.m_dataTab ) );
    }
    else {
        d->u_dialog.tabWidget->insertTab( dataTabIndex, d->u_dialog.m_dataTab, tr( "Data" ) );
        d->u_dialog.m_pMarbleDataBrowser->setText( data );
    }
}

void PluginAboutDialog::setLicense( PluginAboutDialog::LicenseKey license )
{
    QString filename;
    switch ( license ) {
        case PluginAboutDialog::License_LGPL_V2:
            filename = "lgpl2.txt";
            break;
        default:
            filename = "lgpl2.txt";
    }

    QString path = MarbleDirs::path( "licenses/" + filename );
    QTextBrowser *browser = d->u_dialog.m_pMarbleLicenseBrowser;
    browser->setText( QString() );
    if( !path.isEmpty() )
    {
        QFile  f( path );
        if( f.open( QIODevice::ReadOnly ) )
        {
            QTextStream ts( &f );
            browser->setText( ts.readAll() );
        }
        f.close();
    }
}

void PluginAboutDialog::setLicenseAgreementText( const QString& license )
{
    if ( license.isNull() ) {
        setLicense( PluginAboutDialog::License_LGPL_V2 );
    }
    else {
        d->u_dialog.m_pMarbleLicenseBrowser->setText( license );
    }
}

} // namespace Marble

#include "PluginAboutDialog.moc"
