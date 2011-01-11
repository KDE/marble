//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#include "MarbleThemeSelectView.h"

#include "global.h"
#include "MarbleDirs.h"
#include "MapWizard.h"
#include "MarbleDebug.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

using namespace Marble;

class MarbleThemeSelectView::Private
{
public:
    explicit Private( MarbleThemeSelectView * const parent );
    void deleteDirectory( const QString& path );
    void deleteDataDirectories( const QString& path );
    void deletePreview( const QString& path );
    QString currentThemeName();
    QString currentThemePath();
private:
    MarbleThemeSelectView *m_parent;
};

MarbleThemeSelectView::Private::Private( MarbleThemeSelectView * const parent ) 
    : m_parent( parent )
{

}

void MarbleThemeSelectView::Private::deleteDirectory( const QString& path )
{
    QDir directory( path );
    foreach( QString filename, directory.entryList( QDir::Files | QDir::NoDotAndDotDot ) )
        QFile( path + filename ).remove();
    QDir().rmdir( path );
}

void MarbleThemeSelectView::Private::deleteDataDirectories( const QString& path )
{
    QDir directoryv( path );
    foreach( QString filename, directoryv.entryList( QDir::AllEntries | QDir::NoDotAndDotDot ) )
    {
        QString filepath = path + "/" + filename;
        QFile file( filepath );
        if( QFileInfo( filepath ).isDir() && filename.contains( QRegExp( "^[0-9]+$" ) ) )
        {
            deleteDataDirectories( filepath );
            QDir().rmdir( filepath );
        }
        else if( filename.contains( QRegExp( "^[0-9]\\..+" ) ) )
            file.remove();
    }
}

void MarbleThemeSelectView::Private::deletePreview( const QString& path )
{
    QDir directoryv( path, "preview.*" );
    foreach( QString filename, directoryv.entryList() )
        QFile( path + "/" + filename ).remove();
}

QString MarbleThemeSelectView::Private::currentThemeName()
{
    QModelIndex index = m_parent->currentIndex();
    const QAbstractItemModel *model = index.model();

    QModelIndex columnIndex = model->index( index.row(), 0, QModelIndex() );
    return ( model->data( columnIndex )).toString();
}

QString MarbleThemeSelectView::Private::currentThemePath()
{
    QModelIndex index = m_parent-> currentIndex();
    const QAbstractItemModel  *model = index.model();

    QModelIndex columnIndex = model->index( index.row(), 1, QModelIndex() );
    return ( model->data( columnIndex )).toString();
}

MarbleThemeSelectView::MarbleThemeSelectView(QWidget *parent)
    : QListView( parent ),
      d( new Private( this ) )
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        setViewMode( QListView::ListMode );
        setIconSize( QSize( 64, 64 ) );
    } else {
        setViewMode( QListView::IconMode );
        setIconSize( QSize( 136, 136 ) );
        setFlow( QListView::LeftToRight );
        setWrapping( true );
        setResizeMode( QListView::Fixed );
        setUniformItemSizes( true );
    }
    setMovement( QListView::Static );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    setSelectionMode( QAbstractItemView::SingleSelection );

    connect( this, SIGNAL( activated( QModelIndex ) ),
                   SLOT( selectedMapTheme( QModelIndex ) ) );
    connect( this, SIGNAL( customContextMenuRequested( QPoint ) ),
                   SLOT( showContextMenu( QPoint ) ) );
}

MarbleThemeSelectView::~MarbleThemeSelectView()
{
    delete d;
}

void MarbleThemeSelectView::resizeEvent( QResizeEvent *event )
{
    QListView::resizeEvent(event);

    QSize size = gridSize();
    size.setWidth( event->size().width() );
    setGridSize(size);
}

void MarbleThemeSelectView::selectedMapTheme( QModelIndex index )
{
    const QAbstractItemModel *model = index.model();

    QModelIndex  columnIndex = model->index( index.row(), 1, QModelIndex() );
    QString currentmaptheme = ( model->data( columnIndex )).toString();
    mDebug() << currentmaptheme;
    emit selectMapTheme( currentmaptheme );
}

void MarbleThemeSelectView::mapWizard()
{
    emit showMapWizard();
}

void MarbleThemeSelectView::uploadDialog()
{
    emit showUploadDialog();
}

void MarbleThemeSelectView::showContextMenu( const QPoint& pos )
{
    QMenu menu;
    menu.addAction( "&Create a New Map...", this, SLOT( mapWizard() ) );
    if( QFileInfo( MarbleDirs::localPath() + "/maps/" + d->currentThemePath() ).exists() )
        menu.addAction( tr( "&Delete Map Theme" ), this, SLOT( deleteMap() ) );
    menu.addAction( "&Upload Map...", this, SLOT( uploadDialog() ) );
    menu.exec( mapToGlobal( pos ) );
}

void MarbleThemeSelectView::deleteMap()
{
    if(QMessageBox::warning( this, 
                             tr( "Marble" ), 
                             tr( "Are you sure that you want to delete \"%1\"?" ).arg( d->currentThemeName() ),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
    {
        QDir mapthemedir( QFileInfo( MarbleDirs::localPath() + "/maps/" + d->currentThemePath()).path());
        d->deleteDirectory( mapthemedir.path() + "/legend/" );
        d->deleteDataDirectories( mapthemedir.path() + "/" );
	d->deletePreview( mapthemedir.path() + "/" );
        QFile( MarbleDirs::localPath() + "/maps/" + d->currentThemePath()).remove();
        QFile( mapthemedir.path() + "/legend.html" ).remove();
        QDir().rmdir( mapthemedir.path() );
    }
}

#include "MarbleThemeSelectView.moc"
