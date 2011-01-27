//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//
//

#include "NewBookmarkFolderDialog.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"

#include <QtGui/QLineEdit>

namespace Marble {

NewBookmarkFolderDialog::NewBookmarkFolderDialog( QWidget *parent )
    : QDialog(parent), m_widget( 0 )
{
    setupUi(this);
    connect( this, SIGNAL( accepted() ), this, SLOT( addNewBookmarkFolder() ) );
}

NewBookmarkFolderDialog::~NewBookmarkFolderDialog()
{
}

void NewBookmarkFolderDialog::setMarbleWidget( MarbleWidget* widget )
{
    m_widget = widget;
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

void NewBookmarkFolderDialog::addNewBookmarkFolder()
{
    if ( m_widget ) {
        mDebug() << " Adding New Bookmark Folder "<< m_name->text() ;
        m_widget->addNewBookmarkFolder( m_name->text() );
    }
}

}

#include "NewBookmarkFolderDialog.moc"
