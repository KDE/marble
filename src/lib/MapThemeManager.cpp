//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


// Own
#include "MapThemeManager.h"

// Qt
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QStandardItemModel>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

// Local dir
#include "GeoSceneDocument.h"
#include "GeoSceneParser.h"
#include "MarbleDirs.h"


MapThemeManager::MapThemeManager(QObject *parent)
    : QObject(parent)
{
    m_mapThemeModel = new QStandardItemModel( 0, 3 );

    m_mapThemeModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_mapThemeModel->setHeaderData(1, Qt::Horizontal, tr("Description"));
    m_mapThemeModel->setHeaderData(2, Qt::Horizontal, tr("Path"));

    updateMapThemeModel();
/*
//  Should we put this into 
//  static QString MapThemeManager::suggestTheme( QString themeSuggestedBySettings ); 
//  ??

    QStringList  mapthemedirs = findMapThemes( "maps/" );
    QString      selectedmap;

    // We need at least one maptheme to run Marble.
    if ( mapthemedirs.count() == 0 ) {
        qDebug() << "Could not find any maps! Exiting ...";
        exit(-1);
    }

    // If any map directories were found, try to find the default map:
    // srtm.  If we can find that, just grab the first one.
    if ( mapthemedirs.count() >= 1 ) {
        QStringList  tmp = mapthemedirs.filter( "srtm.dgml" );
        if ( tmp.count() >= 1 )
            selectedmap = tmp[0];
        else
            selectedmap = mapthemedirs[0];
    }

//    setMapTheme( selectedmap, parent, Spherical );
*/
}

MapThemeManager::~MapThemeManager()
{
    delete m_mapThemeModel;
}

GeoSceneDocument* MapThemeManager::loadMapTheme( const QString& mapThemeStringID )
{
    // Read the maptheme into d->m_maptheme.
    QString mapThemePath = QString("maps/%1").arg( mapThemeStringID );


    // Check whether file exists
    QFile file( MarbleDirs::path( mapThemePath ) );
    if (!file.exists()) {
        qDebug() << "File does not exist:" << MarbleDirs::path( mapThemePath );
        return 0;
    }

    // Open file in right mode
    file.open(QIODevice::ReadOnly);

    GeoSceneParser* parser = new GeoSceneParser(GeoScene_DGML);;

    if (!parser) {
        qDebug("Could not determine file format!");
        return 0;
    }

    if (!parser->read(&file)) {
        qDebug("Could not parse file!");
        return 0;
    }

    // Get result document
    GeoSceneDocument* document = static_cast<GeoSceneDocument*>(parser->releaseDocument());
    Q_ASSERT(document);

    qDebug() << "\nSuccesfully parsed file!";

    delete parser;

    return document;
}

QStringList MapThemeManager::findMapThemes( const QString& path )
{
    QDir  localPaths = QDir( MarbleDirs::localPath()  + '/' + path );
    QDir  sysdirs    = QDir( MarbleDirs::systemPath() + '/' + path );

    QStringList  localmappaths = localPaths.entryList( QStringList( "*" ),
                                                       QDir::AllDirs
                                                       | QDir::NoSymLinks
                                                       | QDir::NoDotAndDotDot );
    QStringList  sysmappaths = sysdirs.entryList( QStringList( "*" ),
                                                  QDir::AllDirs
                                                  | QDir::NoSymLinks
                                                  | QDir::NoDotAndDotDot );


    QStringList  localmapdirs;
    QStringList  sysmapdirs;

    for ( int planet = 0; planet < localmappaths.size(); ++planet ) {

        QDir themeDir = QDir( MarbleDirs::localPath() + "/maps/"
+ localmappaths.at( planet ) );
        QStringList thememappaths = themeDir.entryList( 
                                     QStringList( "*" ),
                                     QDir::AllDirs |
                                     QDir::NoSymLinks |
                                     QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < thememappaths.size(); ++theme ) {
            localmapdirs << MarbleDirs::localPath() + "/maps/" +
            localmappaths.at( planet ) + '/' + 
            thememappaths.at( theme );
        }
    }

    for ( int planet = 0; planet < sysmappaths.size(); ++planet ) {

        QDir themeDir = QDir( MarbleDirs::systemPath() + "/maps/"
+ sysmappaths.at( planet ) );
        QStringList thememappaths =  themeDir.entryList( 
                                     QStringList( "*" ),
                                     QDir::AllDirs |
                                     QDir::NoSymLinks |
                                     QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < thememappaths.size(); ++theme ) {
            sysmapdirs << MarbleDirs::systemPath() + "/maps/" +
            sysmappaths.at( planet ) + '/' + 
            thememappaths.at( theme );
        }
    }
        
    QStringList  mapfiles;
    QStringList  tmp;
    QString      themedir;
    QString      themedirname;
    QString      themexml;

    QStringListIterator  it( localmapdirs );
    while ( it.hasNext() ) {
        themedir = it.next() + '/';
        themedirname = QDir( themedir ).path().section( "/", -2, -1);

        tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgm2" ),
                                              QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  k( tmp );
            while ( k.hasNext() ) {
                themexml = k.next();
                mapfiles << themedirname + '/' + themexml;
            }
        }
    }

    QStringListIterator  j( sysmapdirs );
    while ( j.hasNext() ) {
        themedir = j.next();
        themedirname = QDir( themedir ).path().section( "/", -2, -1);

        tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgm2" ),
                                              QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  l( tmp );
            while ( l.hasNext() ) {
                themexml = l.next();
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

//     for (int i = 0; i < mapfiles.size(); ++i)
//    	   qDebug() << "Files: " << mapfiles.at(i);

    return mapfiles;
}

QStandardItemModel* MapThemeManager::mapThemeModel()
{
    return m_mapThemeModel;
}

void MapThemeManager::updateMapThemeModel()
{
    QStringList stringlist = findMapThemes( "maps/" );
    QStringListIterator  it(stringlist);

    // Make sure we don't keep excessively large previews in memory
    // TODO: Scale the icon down to the default icon size in katlasselectview.
    //       For now maxIconSize already equals what's expected by the listview.
    QSize maxIconSize( 136,136 ); 

    int  row = 0;
    while ( it.hasNext() ) {
        QString mapThemeID = it.next();
        qDebug() << "About to add: " << mapThemeID;

        GeoSceneDocument *mapTheme = loadMapTheme( mapThemeID );
        
        if ( !mapTheme ) {
            delete mapTheme;
            continue;
        }

        QPixmap themeIconPixmap;
        QString relativePath;

        relativePath = "maps/" 
                     + mapTheme->head()->target() + "/" + mapTheme->head()->theme() + "/" 
                     + mapTheme->head()->icon()->pixmap();
        themeIconPixmap.load( MarbleDirs::path( relativePath ) );

        if ( themeIconPixmap.isNull() ) {
            relativePath = "svg/application-x-marble-gray.png"; 
            themeIconPixmap.load( MarbleDirs::path( relativePath ) );
        }
        else {
            themeIconPixmap = themeIconPixmap.scaled( maxIconSize, 
                              Qt::KeepAspectRatio, Qt::SmoothTransformation );
        } 

        QIcon mapThemeIcon =  QIcon(themeIconPixmap);

        QString name = mapTheme->head()->name();
        QString description = mapTheme->head()->description();

        QList<QStandardItem *> itemList;
        QStandardItem *item = new QStandardItem( name );
        item->setData( name, Qt::DisplayRole );
        item->setData( mapThemeIcon, Qt::DecorationRole );
        item->setData( QString( "<span style=\" max-width: 150 px;\"> " 
                       + tr( description.toUtf8() ) + " </span>"), Qt::ToolTipRole);

        itemList << item;
        itemList << new QStandardItem( description );
        itemList << new QStandardItem( mapTheme->head()->target() + "/" 
                                       + mapTheme->head()->theme() );

        m_mapThemeModel->appendRow(itemList);

        delete mapTheme;
    }
}


#include "MapThemeManager.moc"
