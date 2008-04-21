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
#include <QtCore/QTimer>
#include <QtCore/QDebug>

// Local dir
#include "GeoSceneDocument.h"
#include "GeoSceneParser.h"
#include "MarbleDirs.h"


MapThemeManager::MapThemeManager(QObject *parent)
    : QObject(parent)
{
    m_mapThemeModel = new QStandardItemModel( 0, 3 );

    // Delayed model initialization
    QTimer::singleShot( 0, this, SLOT( updateMapThemeModel() ) );
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

    GeoSceneParser parser(GeoScene_DGML);

    if (!parser.read(&file)) {
        qDebug("Could not parse file!");
        return 0;
    }

    // Get result document
    GeoSceneDocument* document = static_cast<GeoSceneDocument*>(parser.releaseDocument());
    Q_ASSERT(document);

    qDebug() << "\nSuccesfully parsed file!";

    return document;
}

QStringList MapThemeManager::findMapThemes( const QString& basePath )
{
    const QString mapDirName = "maps/";
    const QString mapPathName = basePath + '/' + mapDirName;

    QDir paths = QDir( mapPathName );

    QStringList mapPaths = paths.entryList( QStringList( "*" ),
                                            QDir::AllDirs
                                            | QDir::NoSymLinks
                                            | QDir::NoDotAndDotDot );
    QStringList mapDirs;

    for ( int planet = 0; planet < mapPaths.size(); ++planet ) {

        QDir themeDir = QDir( mapPathName + mapPaths.at( planet ) );
        QStringList themeMapPaths = themeDir.entryList( 
                                     QStringList( "*" ),
                                     QDir::AllDirs |
                                     QDir::NoSymLinks |
                                     QDir::NoDotAndDotDot );
        for ( int theme = 0; theme < themeMapPaths.size(); ++theme ) {
            mapDirs << mapPathName + mapPaths.at( planet ) + '/'
                + themeMapPaths.at( theme );
        }
    }


    QStringList mapFiles;
    QStringListIterator it( mapDirs );
    while ( it.hasNext() ) {
        QString themeDir = it.next() + '/';
        QString themeDirName = QDir( themeDir ).path().section( "/", -2, -1);
        QStringList tmp = ( QDir( themeDir ) ).entryList( QStringList( "*.dgml" ),
                                              QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator k( tmp );
            while ( k.hasNext() ) {
                QString themeXml = k.next();
                mapFiles << themeDirName + '/' + themeXml;
            }
        }
    }

//     for (int i = 0; i < mapFiles.size(); ++i)
//         qDebug() << basePath << "-Files: " << mapFiles.at(i);

    return mapFiles;
}

QStringList MapThemeManager::findMapThemes()
{
    QStringList mapFilesLocal = findMapThemes( MarbleDirs::localPath() );
    QStringList mapFilesSystem = findMapThemes( MarbleDirs::systemPath() );
    QStringList allMapFiles( mapFilesLocal );
    allMapFiles << mapFilesSystem;

    // remove duplicate entries
    allMapFiles.sort();
    for ( int i = 1; i < allMapFiles.size(); ++i ) {
        if ( allMapFiles.at(i) == allMapFiles.at( i - 1 ) ) {
            allMapFiles.removeAt(i);
            --i;
        }
    }

    for (int i = 0; i < allMapFiles.size(); ++i)
       qDebug() << "Files: " << allMapFiles.at(i);

    return allMapFiles;
}

QStandardItemModel* MapThemeManager::mapThemeModel()
{
    return m_mapThemeModel;
}

void MapThemeManager::updateMapThemeModel()
{
    m_mapThemeModel->clear();

    m_mapThemeModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_mapThemeModel->setHeaderData(1, Qt::Horizontal, tr("Path"));
    m_mapThemeModel->setHeaderData(2, Qt::Horizontal, tr("Description"));

    QStringList stringlist = findMapThemes();
    QStringListIterator  it(stringlist);

    // Make sure we don't keep excessively large previews in memory
    // TODO: Scale the icon down to the default icon size in katlasselectview.
    //       For now maxIconSize already equals what's expected by the listview.
    QSize maxIconSize( 136,136 ); 

    while ( it.hasNext() ) {
        QString mapThemeID = it.next();
        qDebug() << "About to add: " << mapThemeID;

        GeoSceneDocument *mapTheme = loadMapTheme( mapThemeID );
        
        if ( !mapTheme ) {
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
        itemList << new QStandardItem( mapTheme->head()->target() + "/" 
                                       + mapTheme->head()->theme() + "/"
                                       + mapTheme->head()->theme() + ".dgml" );
        itemList << new QStandardItem( description );

        m_mapThemeModel->appendRow(itemList);

        delete mapTheme;
    }
}

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

#include "MapThemeManager.moc"
