//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>
//
//

#include "NewBookmarkFolderDialog.h"
#include "MarbleDebug.h"

namespace Marble {

NewBookmarkFolderDialog::NewBookmarkFolderDialog( QWidget *parent )
    : QDialog(parent)
{
    setupUi(this);
}

NewBookmarkFolderDialog::~NewBookmarkFolderDialog()
{
}

QString NewBookmarkFolderDialog::folderName() const
{
    return m_name->text();
}

void NewBookmarkFolderDialog::setFolderName( const QString &name )
{
    m_name->setText( name );
    m_name->selectAll();
}

}

#include "moc_NewBookmarkFolderDialog.cpp"
