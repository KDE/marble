//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "MapThemeDownloadDialog.h"
#include "ui_MapThemeDownloadDialog.h"

#include "MarbleDirs.h"
#include "NewstuffModel.h"

namespace Marble
{

class MapThemeDownloadDialog::Private : public Ui::MapThemeDownloadDialog
{
public:
    Private() :
        m_model()
    {}

    NewstuffModel m_model;
};

MapThemeDownloadDialog::MapThemeDownloadDialog( QWidget* parent ) :
    QDialog( parent ),
    d( new Private )
{
    d->setupUi( this );

    d->m_model.setTargetDirectory( MarbleDirs::localPath() + "/maps" );
    d->m_model.setProvider( "http://edu.kde.org/marble/newstuff/maps.xml" );

    d->listView->setModel( &d->m_model );
}

MapThemeDownloadDialog::~MapThemeDownloadDialog()
{
    delete d;
}

}

#include "MapThemeDownloadDialog.moc"
