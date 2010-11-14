//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "GeoSceneMapTheme.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

#include "MarbleDebug.h"
#include "MarbleDirs.h"

namespace Marble
{

// ================================================================
//                           Legend classes



GeoSceneLegendItem::GeoSceneLegendItem()
  : m_background( Qt::transparent ),
    m_symbol(),
    m_text()
{
}


// ================================================================
//                         class GeoSceneMapTheme


GeoSceneMapTheme::GeoSceneMapTheme(QObject *parent)
    : QObject(parent)
{
}

GeoSceneMapTheme::~GeoSceneMapTheme()
{
    qDeleteAll( m_legend );
    m_legend.clear();
}

QStringList GeoSceneMapTheme::findGeoSceneMapThemes( const QString& path )
{
    QDir localPaths = QDir( MarbleDirs::localPath()  + '/' + path );
    QDir sysdirs    = QDir( MarbleDirs::systemPath() + '/' + path );

    QStringList localmappaths = localPaths.entryList( QStringList("*"),
                                                      QDir::AllDirs
                                                      | QDir::NoSymLinks
                                                      | QDir::NoDotAndDotDot );
    QStringList sysmappaths = sysdirs.entryList( QStringList( "*" ),
                                                 QDir::AllDirs
                                                 | QDir::NoSymLinks
                                                 | QDir::NoDotAndDotDot );

    QStringList localmapdirs;
    QStringList sysmapdirs;

    for ( int planet = 0; planet < localmappaths.size(); ++planet ) {
        QDir themeDir = QDir( MarbleDirs::localPath() + "/maps/" + localmappaths.at( planet ) );
        QStringList thememappaths = themeDir.entryList( QStringList( "*" ),
                                                        QDir::AllDirs
                                                        | QDir::NoSymLinks
                                                        | QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < thememappaths.size(); ++theme ) {
            localmapdirs << ( MarbleDirs::localPath() + "/maps/" + localmappaths.at( planet ) + '/'
                              + thememappaths.at( theme ) );
        }
    }

    for ( int planet = 0; planet < sysmappaths.size(); ++planet ) {
        QDir themeDir = QDir( MarbleDirs::systemPath() + "/maps/" + sysmappaths.at( planet ) );
        QStringList thememappaths = themeDir.entryList( QStringList( "*" ),
                                                        QDir::AllDirs
                                                        | QDir::NoSymLinks
                                                        | QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < thememappaths.size(); ++theme ) {
            sysmapdirs << ( MarbleDirs::systemPath() + "/maps/" + sysmappaths.at( planet ) + '/'
                            + thememappaths.at( theme ) );
        }
    }

    QStringList  mapfiles;

    QStringListIterator  it( localmapdirs );
    while ( it.hasNext() ) {
        QString themedir = it.next() + '/';
        QString themedirname = QDir( themedir ).dirName();

        QStringList tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                                          QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  k( tmp );
            while ( k.hasNext() ) {
                QString themexml = k.next();
                mapfiles << themedirname + '/' + themexml;
            }
        }
    }

    QStringListIterator  j( sysmapdirs );
    while ( j.hasNext() ) {
        QString themedir = j.next();
        QString themedirname = QDir( themedir ).path().section( '/', -2, -1 );

        QStringList tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                                          QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  l( tmp );
            while ( l.hasNext() ) {
                QString themexml = l.next();
                mapfiles << themedirname + '/' + themexml;
            }
        }
    }

    mapfiles.sort();

    for ( int i = 1; i < mapfiles.size(); ++i ) {
        if ( mapfiles.at(i) == mapfiles.at( i - 1 ) ) {
            mapfiles.removeAt(i);
            --i;
        }
    }

    // for (int i = 0; i < mapfiles.size(); ++i)
    //	   mDebug() << "Files: " << mapfiles.at(i);

    return mapfiles;
}


QStandardItemModel* GeoSceneMapTheme::mapThemeModel( const QStringList& stringlist )
{
    QStandardItemModel  *mapthememodel = new QStandardItemModel();

    QModelIndex  parent;
    mapthememodel->insertColumns(0, 3, parent);

    mapthememodel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    mapthememodel->setHeaderData(1, Qt::Horizontal, tr("Description"));
    mapthememodel->setHeaderData(2, Qt::Horizontal, tr("Path"));

    QStringListIterator  it(stringlist);
    GeoSceneMapTheme *maptheme = new GeoSceneMapTheme();

    // Make sure we don't keep excessively large previews in memory
    // TODO: Scale the icon down to the default icon size in katlasselectview.
    //       For now maxIconSize already equals what's expected by the listview.
    QSize maxIconSize( 136, 136 );

    int  row = 0;
    while ( it.hasNext() ) {
        QString currentmaptheme = it.next();

//        maptheme->open( MarbleDirs::path( "maps/" + currentmaptheme ) );

        mapthememodel->insertRows( row, 1, QModelIndex() );
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                tr( maptheme->name().toUtf8() ),
                                Qt::DisplayRole );

        QString relativePath = "maps/" + maptheme->prefix() + '/' + maptheme->icon();
        QPixmap themeIconPixmap;
        themeIconPixmap.load( MarbleDirs::path( relativePath ) );

        if ( themeIconPixmap.isNull() ) {
            relativePath = "svg/application-x-marble-gray.png";
            themeIconPixmap.load( MarbleDirs::path( relativePath ) );
        }
        else {
            themeIconPixmap = themeIconPixmap.scaled( maxIconSize,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation );
        }

        QIcon mapThemeIcon =  QIcon(themeIconPixmap);

        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                mapThemeIcon, Qt::DecorationRole );
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                QString( "<span style=\" max-width: 150 px;\"> "
                                + tr( maptheme->description().toLatin1() ) + " </span>"),
                                Qt::ToolTipRole );
        mapthememodel->setData( mapthememodel->index( row, 1, QModelIndex() ),
                                currentmaptheme );
    }

    delete maptheme;

    return mapthememodel;
}

}

#include "GeoSceneMapTheme.moc"
