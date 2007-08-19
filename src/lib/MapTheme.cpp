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

#include "MapTheme.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

#include "MarbleDirs.h"
#include "TileLoader.h"


MapTheme::MapTheme(QObject *parent)
    : QObject(parent)
{
}


int MapTheme::open( const QString& path )
{
    QDomDocument  doc( "DeskGlobeML" );
    QFileInfo     fileInfo(path);
    if ( !fileInfo.isFile() ) {
        qDebug() << QString("Not a valid maptheme file: ") + path;
        return -4;
    }
      
    
    QFile         file( path );

    if ( !file.open( QIODevice::ReadOnly ) ) {
        qDebug() << QString("Could not open ") + path;
        return -1;
    }

    if ( !doc.setContent( &file ) ) {
        qDebug() << QString("Parse error!");
        qDebug() << "Path: " << path;
        file.close();
        return -2;
    }
    file.close();

    QDomElement  element = doc.documentElement();
    if ( element.tagName() != "DeskGlobeML" ) {
        qDebug("File is not a DeskGlobe file");
        return -3;
    }

    m_labelColor = QColor( 0, 0, 0, 255 );

    m_oceanColor = QColor( 0, 0, 0, 255 );
    m_landColor  = QColor( 0, 0, 0, 255 );
    m_countryBorderColor = QColor( 0, 0, 0, 255 );
    m_stateBorderColor   = QColor( 0, 0, 0, 255 );
    m_lakeColor  = QColor( 0, 0, 0, 255 );
    m_riverColor = QColor( 0, 0, 0, 255 );

    m_bitmaplayer.enabled = false;
    m_bitmaplayer.name    = "";
    m_bitmaplayer.dem     = "false";

    m_vectorlayer.enabled = false;
    m_vectorlayer.name    = "";

    element = element.firstChildElement();
    while ( !element.isNull() ) {

        if ( element.tagName().toLower() == "mapstyle" ) {
            // qDebug("Parsing MapStyleNode");

            QDomElement  mapStyleSibling = element.firstChildElement();
            while ( !mapStyleSibling.isNull() ) {

                if ( mapStyleSibling.tagName().toLower() == "name" ) {
                    m_name = mapStyleSibling.text();
//                    qDebug() << "Parsed Name: " << m_name;
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

                if ( mapStyleSibling.tagName().toLower() == "labelstyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "color" ) {
                            m_labelColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "oceanstyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "color" ) {
                            m_oceanColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "landstyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "color" ) {
                            m_landColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "countrystyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "border-color" ) {
                            m_countryBorderColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "statestyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "border-color" ) {
                            m_stateBorderColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "lakestyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "color" ) {
                            m_lakeColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "riverstyle" ) {
                    QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
                    while ( !labelStyleSibling.isNull() ) {
                        if ( labelStyleSibling.tagName().toLower() == "color" ) {
                            m_riverColor.setNamedColor( labelStyleSibling.text() );
                        }
                        labelStyleSibling = labelStyleSibling.nextSiblingElement();
                    }
                }

                if ( mapStyleSibling.tagName().toLower() == "layer" ) {

                    if ( mapStyleSibling.attribute( "type", "" ) == "bitmap" )
                    {
                        m_bitmaplayer.enabled = true;
                        m_bitmaplayer.name    = mapStyleSibling.attribute( "name", "" );
                        m_bitmaplayer.type    = mapStyleSibling.attribute( "type", "" );
                        m_bitmaplayer.dem     = mapStyleSibling.attribute( "dem", "" );
                    }

                    if ( mapStyleSibling.attribute( "type", "" ) == "vector" )
                    {
                        m_vectorlayer.enabled = true;
                        m_vectorlayer.name    = mapStyleSibling.attribute( "name", "" );
                        m_vectorlayer.type    = mapStyleSibling.attribute( "type", "" );
                    }					
                }

                mapStyleSibling = mapStyleSibling.nextSiblingElement();
            }
        }

        element = element.nextSiblingElement();
    }

    return 0;
}


QStringList MapTheme::findMapThemes( const QString& path )
{
    QDir  localPaths = QDir( MarbleDirs::localPath() + '/' + path );
    QDir  sysdirs   = QDir( MarbleDirs::systemPath() + '/' + path );

    QStringList  localmappaths = localPaths.entryList( QStringList( "*" ),
                                                      QDir::AllDirs
                                                      | QDir::NoSymLinks
                                                      | QDir::NoDotAndDotDot );
    QStringList sysmappaths = sysdirs.entryList( QStringList( "*" ),
                                                 QDir::AllDirs
                                                 | QDir::NoSymLinks
                                                 | QDir::NoDotAndDotDot );


    QStringList  localmapdirs;
    QStringList  sysmapdirs;
    QStringList  unixmapdirs;

    for ( int i = 0; i < localmappaths.size(); ++i ) {
        // qDebug() << "Map dirs: "
        // << MarbleDirs::localPath() + "/maps/" + localmappaths.at(i);
        localmapdirs << MarbleDirs::localPath() + "/maps/earth/" + localmappaths.at(i);
    }

    for ( int i = 0; i < sysmappaths.size(); ++i ) {
        // qDebug() << "Map dirs: " << MarbleDirs::systemPath() + "/maps/" + sysmappaths.at(i);
        sysmapdirs << MarbleDirs::systemPath() + "/maps/earth/" + sysmappaths.at(i);
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

    // Make sure we don't keep excessively large previews in memory
    // TODO: Scale the icon down to the default icon size in katlasselectview.
    //       For now maxIconSize already equals what's expected by the listview.
    QSize maxIconSize( 136,136 ); 

    int  row = 0;
    while ( it.hasNext() ) {
        QString currentmaptheme = it.next();

        maptheme->open( MarbleDirs::path( "maps/earth/" + currentmaptheme ) );

        mapthememodel->insertRows( row, 1, QModelIndex() );
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                tr( maptheme->name().toUtf8() ), 
                                Qt::DisplayRole );
        QIcon mapThemeIcon =  QIcon( QPixmap( MarbleDirs::path( 
                                    "maps/earth/" +  maptheme->prefix() + '/' + maptheme->icon() ) )
                                    .scaled( maxIconSize, 
                                    Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ), mapThemeIcon, 
                                Qt::DecorationRole );
        mapthememodel->setData( mapthememodel->index( row, 0, QModelIndex() ),
                                QString( "<span style=\" max-width: 150 px;\"> " + tr( maptheme->description().toUtf8() ) + " </span>"), 
                                Qt::ToolTipRole);
        mapthememodel->setData( mapthememodel->index( row, 1, QModelIndex() ),
                                currentmaptheme );
    }

    return mapthememodel;
}


#include "MapTheme.moc"
