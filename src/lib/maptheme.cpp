//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "maptheme.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

#include "katlasdirs.h"
#include "TileLoader.h"


MapTheme::MapTheme(QObject *parent)
    : QObject(parent)
{
}


int MapTheme::open( const QString& path )
{
    QDomDocument  doc( "DeskGlobeML" );
    QFile         file( path );

    if ( !file.open( QIODevice::ReadOnly ) ) {
        qDebug() << QString("Could not open ") + path;
        return -1;
    }

    if ( !doc.setContent( &file ) ) {
        qDebug() << QString("Parse error!");
        file.close();
        return -2;
    }
    file.close();

    QDomElement  element = doc.documentElement();
    if ( element.tagName() != "DeskGlobeML" ) {
        qDebug("File is not a DeskGlobe file");
        return -3;
    }

    element = element.firstChildElement();
    while ( !element.isNull() ) {

        if ( element.tagName().toLower() == "mapstyle" ) {
            // qDebug("Parsing MapStyleNode");

            QDomElement  mapStyleSibling = element.firstChildElement();
            while ( !mapStyleSibling.isNull() ) {

                if ( mapStyleSibling.tagName().toLower() == "name" ) {
                    m_name = mapStyleSibling.text();
                    // qDebug() << m_name;
                }

                if ( mapStyleSibling.tagName().toLower() == "prefix" ) {
                    m_prefix = mapStyleSibling.text();
                    // qDebug() << m_prefix;
                }

                if ( mapStyleSibling.tagName().toLower() == "icon" ) {
                    m_icon = mapStyleSibling.text();
                    // qDebug() << m_icon;
                }
				
                if ( mapStyleSibling.tagName().toLower() == "description" ) {
                    m_description = mapStyleSibling.text();
                    // qDebug() << m_description;
                }

                if ( mapStyleSibling.tagName().toLower() == "installmap" ) {
                    m_installmap = mapStyleSibling.text();
                    // qDebug() << m_installmap;
                }

                m_labelcolor = QColor( 0, 0, 0, 255 );
                if ( mapStyleSibling.tagName().toLower() == "labelstyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "color" ) {
                            m_labelcolor.setNamedColor( '#' + labelStyleSibling.text() );
                            // qDebug() << "#" + labelStyleSibling.text();
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                m_labelcolor.setAlpha(255);
                if ( mapStyleSibling.tagName().toLower() == "layer" ) {

                    m_bitmaplayer.enabled = false;
                    if ( mapStyleSibling.attribute( "type", "" ) == "bitmap" ) {
                        m_bitmaplayer.enabled = true;
                        m_bitmaplayer.name    = mapStyleSibling.attribute( "name", "" );
                        m_bitmaplayer.type    = mapStyleSibling.attribute( "type", "" );
                        m_bitmaplayer.dem     = mapStyleSibling.attribute( "dem", "" );
                        if ( m_bitmaplayer.dem.isEmpty() )
                            m_bitmaplayer.dem = "false";
                    }

                    m_vectorlayer.enabled = false;
                    if ( mapStyleSibling.attribute( "type", "" ) == "vector" ) {
                        m_vectorlayer.enabled = true;
                        m_vectorlayer.name    = mapStyleSibling.attribute( "name", "" );
                        m_vectorlayer.type    = mapStyleSibling.attribute( "type", "" );
                    }					
                    // qDebug() << m_layer.name << " " << m_layer.type;
                }

                mapStyleSibling = mapStyleSibling.nextSiblingElement();
            }
        }

        element = element.nextSiblingElement();
    }

    // detectMaxTileLevel();

    return 0;
}


QStringList MapTheme::findMapThemes( const QString& path )
{
    QDir  localdirs = QDir( KAtlasDirs::localDir() + '/' + path );
    QDir  sysdirs   = QDir( KAtlasDirs::systemDir() + '/' + path );
    QDir  unixdirs  = QDir( KAtlasDirs::unixDir() + '/' + path );

    QStringList  localmappaths = localdirs.entryList( QStringList( "*" ),
                                                      QDir::AllDirs
                                                      | QDir::NoSymLinks
                                                      | QDir::NoDotAndDotDot );
    QStringList sysmappaths = sysdirs.entryList( QStringList( "*" ),
                                                 QDir::AllDirs
                                                 | QDir::NoSymLinks
                                                 | QDir::NoDotAndDotDot );
    QStringList unixmappaths = unixdirs.entryList( QStringList( "*" ),
                                                   QDir::AllDirs
                                                   | QDir::NoSymLinks
                                                   | QDir::NoDotAndDotDot );


    QStringList  localmapdirs;
    QStringList  sysmapdirs;
    QStringList  unixmapdirs;

    for ( int i = 0; i < localmappaths.size(); ++i ) {
        // qDebug() << "Map dirs: "
        // << KAtlasDirs::localDir() + "/maps/" + localmappaths.at(i);
        localmapdirs << KAtlasDirs::localDir() + "/maps/earth/" + localmappaths.at(i);
    }

    for ( int i = 0; i < sysmappaths.size(); ++i ) {
        // qDebug() << "Map dirs: " << KAtlasDirs::systemDir() + "/maps/" + sysmappaths.at(i);
        sysmapdirs << KAtlasDirs::systemDir() + "/maps/earth/" + sysmappaths.at(i);
    }

    for ( int i = 0; i < unixmappaths.size(); ++i ) {
        // qDebug() << "Map dirs: " << KAtlasDirs::unixDir() + "/maps/" + unixmappaths.at(i);
        unixmapdirs << KAtlasDirs::unixDir() + "/maps/earth/" + unixmappaths.at(i);
    }
        
    QStringList  mapfiles;
    QStringList  tmp;
    QString      themedir;
    QString      themedirname;
    QString      themexml;

    QStringListIterator  it( localmapdirs );
    while ( it.hasNext() ) {
        themedir = it.next() + '/';
        themedirname = QDir( themedir ).dirName();
        // qDebug() << themedir;

        tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                              QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  k( tmp );
            while ( k.hasNext() ) {
                themexml = k.next();
                // qDebug() << themedirname + "/" + themexml;
                mapfiles << themedirname + '/' + themexml;
            }
        }
        // else qDebug("Empty *.dgml list!");
    }

    QStringListIterator  j( sysmapdirs );
    while ( j.hasNext() ) {
        themedir = j.next();
        // qDebug() << themedir;
        themedirname = QDir( themedir ).dirName();

        tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                              QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  l( tmp );
            while ( l.hasNext() ) {
                themexml = l.next();
                // qDebug() << themedirname + "/" + themexml;
                mapfiles << themedirname + '/' + themexml;
            }
        }
        // else qDebug("Empty *.dgml list!");
    }

    QStringListIterator  k( unixmapdirs );
    while ( k.hasNext() ) {
        themedir = k.next();
        // qDebug() << themedir;
        themedirname = QDir( themedir ).dirName();

        tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                              QDir::Files | QDir::NoSymLinks );
        if ( !tmp.isEmpty() ) {
            QStringListIterator  l( tmp );
            while ( l.hasNext() ) {
                themexml = l.next();
                // qDebug() << themedirname + "/" + themexml;
                mapfiles << themedirname + '/' + themexml;
            }
        }
        // else qDebug("Empty *.dgml list!");
    }

    mapfiles.sort();

    for ( int i = 1; i < mapfiles.size(); ++i ) {
        if ( mapfiles.at(i) == mapfiles.at( i - 1 ) ) {
            mapfiles.removeAt(i);
            --i;
        }
    }

    // for (int i = 0; i < mapfiles.size(); ++i)
    //	   qDebug() << "Files: " << mapfiles.at(i);

    return mapfiles;
}


QStandardItemModel* MapTheme::mapThemeModel( const QStringList& stringlist )
{
    QStandardItemModel  *mapthememodel = new QStandardItemModel();

    QModelIndex  parent;
    mapthememodel->insertColumns(0, 3, parent);

    mapthememodel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    mapthememodel->setHeaderData(1, Qt::Horizontal, tr("Description"));
    mapthememodel->setHeaderData(2, Qt::Horizontal, tr("Path"));

    QStringListIterator  it(stringlist);
    MapTheme            *maptheme = new MapTheme();

    int  row = 0;
    while ( it.hasNext() ) {
        QString currentmaptheme = it.next();
        maptheme->open( KAtlasDirs::path( "maps/earth/" + currentmaptheme ) );

        mapthememodel->insertRows(row, 1, QModelIndex());
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                tr( maptheme->name().toUtf8() ), Qt::DisplayRole );
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                QIcon( KAtlasDirs::path( "maps/earth/" +  maptheme->prefix() + '/' + maptheme->icon() ) ), 
                                Qt::DecorationRole );
        mapthememodel->setData( mapthememodel->index( row, 1, QModelIndex() ),
                                tr( maptheme->description().toUtf8() ), Qt::ToolTipRole);
        mapthememodel->setData( mapthememodel->index( row, 2, QModelIndex() ),
                                currentmaptheme );
    }

    return mapthememodel;
}


#include "maptheme.moc"
