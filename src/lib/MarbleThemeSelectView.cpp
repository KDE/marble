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

#include "MarbleGlobal.h"
#include "MarbleDirs.h"
#include "MapWizard.h"
#include "MarbleDebug.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QModelIndex>
#include <QtCore/QSettings>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QStandardItemModel>

using namespace Marble;

class MarbleThemeSelectView::Private
{
public:
    explicit Private( MarbleThemeSelectView * const parent );
    void deleteDirectory( const QString& path );
    void deleteDataDirectories( const QString& path );
    void deletePreview( const QString& path );
    void loadFavorites();
    bool currentIsFavorite();

    void selectedMapTheme( QModelIndex index );
    void uploadDialog();
    void mapWizard();
    void showContextMenu( const QPoint& pos );
    void deleteMap();
    void toggleFavorite();

    QString currentThemeName() const;
    QString currentThemePath() const;
private:
    MarbleThemeSelectView *m_parent;
    QSettings m_settings;
};

MarbleThemeSelectView::Private::Private( MarbleThemeSelectView * const parent ) 
    : m_parent( parent ),
      m_settings( "kde.org", "Marble Desktop Globe" )
{

}

void MarbleThemeSelectView::Private::deleteDirectory( const QString& path )
{
    QDir directory( path );
    foreach( const QString &filename, directory.entryList( QDir::Files | QDir::NoDotAndDotDot ) )
        QFile( path + filename ).remove();
    QDir().rmdir( path );
}

void MarbleThemeSelectView::Private::deleteDataDirectories( const QString& path )
{
    QDir directoryv( path );
    foreach( const QString &filename, directoryv.entryList( QDir::AllEntries | QDir::NoDotAndDotDot ) )
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
    foreach( const QString &filename, directoryv.entryList() )
        QFile( path + "/" + filename ).remove();
}

QString MarbleThemeSelectView::Private::currentThemeName() const
{
    QModelIndex index = m_parent->currentIndex();
    const QAbstractItemModel *model = index.model();

    QModelIndex columnIndex = model->index( index.row(), 0, QModelIndex() );
    return ( model->data( columnIndex )).toString();
}

QString MarbleThemeSelectView::Private::currentThemePath() const
{
    QModelIndex index = m_parent->currentIndex();
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
    d->loadFavorites();

#ifdef Q_WS_MAEMO_5
    // The pressed signal on Maemo interfers with touch-based
    // scrolling, leading to sluggish behavior
    connect( this, SIGNAL( activated( QModelIndex ) ),
                   SLOT( selectedMapTheme( QModelIndex ) ) );
#else //  Q_WS_MAEMO_5
    connect( this, SIGNAL( pressed( QModelIndex ) ),
                   SLOT( selectedMapTheme( QModelIndex ) ) );
#endif //  Q_WS_MAEMO_5
    connect( this, SIGNAL( customContextMenuRequested( QPoint ) ),
                   SLOT( showContextMenu( QPoint ) ) );

    setResizeMode( Adjust );
}

MarbleThemeSelectView::~MarbleThemeSelectView()
{
    delete d;
}

void MarbleThemeSelectView::Private::selectedMapTheme( QModelIndex index )
{
    const QAbstractItemModel *model = index.model();

    QModelIndex  columnIndex = model->index( index.row(), 1, QModelIndex() );
    QString currentmaptheme = ( model->data( columnIndex )).toString();
    mDebug() << currentmaptheme;
    emit m_parent->mapThemeIdChanged( currentmaptheme );
}

void MarbleThemeSelectView::Private::mapWizard()
{
    emit m_parent->showMapWizard();
}

void MarbleThemeSelectView::Private::uploadDialog()
{
    emit m_parent->showUploadDialog();
}

void MarbleThemeSelectView::Private::showContextMenu( const QPoint& pos )
{
    QMenu menu;
    menu.addAction( "&Create a New Map...", m_parent, SLOT( mapWizard() ) );
    if( QFileInfo( MarbleDirs::localPath() + "/maps/" + currentThemePath() ).exists() )
        menu.addAction( tr( "&Delete Map Theme" ), m_parent, SLOT( deleteMap() ) );
    menu.addAction( tr( "&Upload Map..." ), m_parent, SLOT( uploadDialog() ) );
    QAction *favAction = menu.addAction( tr( "&Favorite" ), m_parent, SLOT( toggleFavorite() ) );
    favAction->setCheckable( true );
    if( currentIsFavorite() )
        favAction->setChecked( true );
    else
        favAction->setChecked( false );
    menu.exec( m_parent->mapToGlobal( pos ) );
}

void MarbleThemeSelectView::Private::deleteMap()
{
    if(QMessageBox::warning( m_parent,
                             tr( "Marble" ), 
                             tr( "Are you sure that you want to delete \"%1\"?" ).arg( currentThemeName() ),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
    {
        QDir mapthemedir( QFileInfo( MarbleDirs::localPath() + "/maps/" + currentThemePath()).path());
        deleteDirectory( mapthemedir.path() + "/legend/" );
        deleteDataDirectories( mapthemedir.path() + "/" );
        deletePreview( mapthemedir.path() + "/" );
        QFile( MarbleDirs::localPath() + "/maps/" + currentThemePath()).remove();
        QFile( mapthemedir.path() + "/legend.html" ).remove();
        QDir().rmdir( mapthemedir.path() );
    }
}

void MarbleThemeSelectView::Private::toggleFavorite()
{
    QModelIndex index = m_parent->currentIndex();
    QAbstractItemModel *model = m_parent->model();
    QModelIndex columnIndex = model->index( index.row(), 0 );

    if( currentIsFavorite() )
    {
        m_settings.beginGroup( "Favorites" );
        m_settings.remove( model->data( columnIndex ).toString() );
    }
    else
    {
        m_settings.beginGroup( "Favorites" );
        m_settings.setValue( model->data( columnIndex ).toString(), QDateTime::currentDateTime() );
    }
    m_settings.endGroup();
    model->sort( 0 );
}

void MarbleThemeSelectView::Private::loadFavorites()
{
    m_settings.beginGroup( "Favorites" );
    if( !m_settings.contains( "initialized" ) ) {
        m_settings.setValue( "initialized", true );
        QDateTime currentDateTime = QDateTime::currentDateTime();
        m_settings.setValue( "Atlas", currentDateTime );
        m_settings.setValue( "OpenStreetMap", currentDateTime );
        m_settings.setValue( "Satellite View", currentDateTime );
    }
    m_settings.endGroup();
}

bool MarbleThemeSelectView::Private::currentIsFavorite()
{
    QModelIndex index = m_parent->currentIndex();
    const QAbstractItemModel  *model = index.model();
    QModelIndex nameIndex = model->index( index.row(), 0, QModelIndex() );
    m_settings.beginGroup( "Favorites" );
    bool favorite = m_settings.contains( model->data( nameIndex ).toString() );
    m_settings.endGroup();
    return favorite;
}

#include "MarbleThemeSelectView.moc"
