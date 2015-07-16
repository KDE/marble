//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "ExternalEditorDialog.h"

#include <QProcessEnvironment>
#include <QFileInfo>
#include <QDir>

namespace Marble
{

namespace {
    QString const merkaartor = "merkaartor";
    QString const josm = "josm";
    QString const potlatch = "potlatch";
}

class ExternalEditorDialogPrivate {
public:
    QString m_defaultEditor;

    QMap<QString,bool> m_installedEditors;

    ExternalEditorDialogPrivate();
};

ExternalEditorDialogPrivate::ExternalEditorDialogPrivate() :
        m_defaultEditor( potlatch )
{
    QString path = QProcessEnvironment::systemEnvironment().value( "PATH", "/usr/local/bin:/usr/bin:/bin" );
    foreach( const QString &application, QStringList() << merkaartor << josm ) {
        m_installedEditors[application] = false;
        /** @todo: what's the qt way to get the path entry separator? Will be a semicolon on Windows */
        foreach( const QString &dir, path.split( QLatin1Char( ':' ) ) ) {
            QFileInfo executable( QDir( dir ), application );
            if ( executable.exists() ) {
                m_installedEditors[application] = true;
                break;
            }
        }
    }
}

ExternalEditorDialog::ExternalEditorDialog( QWidget * parent, Qt::WindowFlags flags ) :
        QDialog( parent, flags ), d( new ExternalEditorDialogPrivate )
{
    setupUi( this );

    connect( editorComboBox, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updateDefaultEditor(int)) );

    if ( d->m_installedEditors[merkaartor] ) {
        d->m_defaultEditor = merkaartor;
        editorComboBox->setCurrentIndex( 1 );
    } else if ( d->m_installedEditors[josm] ) {
        d->m_defaultEditor = josm;
        editorComboBox->setCurrentIndex( 2 );
    }
}

ExternalEditorDialog::~ExternalEditorDialog()
{
    delete d;
}

QString ExternalEditorDialog::externalEditor() const
{
    return d->m_defaultEditor;
}

bool ExternalEditorDialog::saveDefault() const
{
    return saveDefaultCheckBox->isChecked();
}

void ExternalEditorDialog::updateDefaultEditor( int index )
{
    QString description;

    switch( index ) {
    case 1:
        d->m_defaultEditor = merkaartor;
        description = tr( "Merkaartor is an OpenStreetMap editor that is powerful and easy to use. It integrates well into the used workspace." );
        if ( !d->m_installedEditors[d->m_defaultEditor] ) {
            description += " <b>" + tr( "Please ask your system administrator to install %1 on your system." ).arg( "Merkaartor") + "</b>";
        }
        break;
    case 2:
        d->m_defaultEditor = josm;
        description = tr( "JOSM is a powerful OpenStreetMap editor which is more complex to use than other editors. It is built on the Java platform and therefor runs on all systems for which Java is available but does not integrate well into the workspace. A Java SE-compatible runtime is required." );
        if ( !d->m_installedEditors[d->m_defaultEditor] ) {
            description += " <b>" + tr( "Please ask your system administrator to install %1 on your system." ).arg( "JOSM" ) + "</b>";
        }
        break;
    default:
        d->m_defaultEditor = potlatch;
        description = tr( "Potlatch is a very easy to use OpenStreetMap editor, though lacks the power of Merkaartor and JOSM. It runs on all platforms for which web browsers with Flash support are available. Performance of Potlatch is depending on the quality of the installed Flash version." );
        break;
    }

    screenshotLabel->setPixmap( QPixmap( ":/data/editors/" + d->m_defaultEditor + ".png" ) );
    descriptionLabel->setText( description );
}

}

#include "moc_ExternalEditorDialog.cpp"
