//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>"
//


// Own
#include "MarbleGeoDataModel.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtCore/QFile>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataDocument.h"       // In geodata/data/
#include "GeoDataContainer.h"
#include "GeoDataParser.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

using namespace Marble;

class MarbleGeoDataModel::Private
{
 public:
    Private()
        : m_rootDocument( new GeoDataDocument() ),
          m_latestId( 0 )
    {
    }

    ~Private()
    {
        delete m_rootDocument;
    }

    GeoDataDocument* m_rootDocument;
    QHash<int, GeoDataDocument*> m_documents;
    unsigned long m_latestId;
};

MarbleGeoDataModel::MarbleGeoDataModel( QObject *parent )
    : QAbstractListModel( parent ), d( new Private() )
{
}

MarbleGeoDataModel::~MarbleGeoDataModel()
{
    delete d;
}

int MarbleGeoDataModel::rowCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() && d->m_rootDocument )
        return d->m_rootDocument->features().size();
    else
        return 0;
}


QVariant MarbleGeoDataModel::data( const QModelIndex &index, int role ) const
{
    Q_UNUSED( index )
    Q_UNUSED( role )

    return QVariant();
}

unsigned long MarbleGeoDataModel::addGeoDataFile( QString filename )
{
    /*
    * read a GeoDataDocument for now - hard coded
    */
    GeoDataParser parser( GeoData_KML );
    
    QFile file( filename );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        return 0;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );
    
    if ( !parser.read( &file ) ) {
        qWarning( "Could not parse file!" );
        return 0;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT_X( document, "geoRoot()", "document unparseable" );
    
    foreach(const GeoDataFeature& feature, static_cast<GeoDataDocument*>( document )->features() ) {
        d->m_rootDocument->append( feature );
    }

    // add this document as a new entry into the hash
    d->m_documents[++(d->m_latestId)] = static_cast<GeoDataDocument*>( document );
    
    // get the styles and the stylemaps
    foreach(const GeoDataStyle& style, static_cast<GeoDataDocument*>( document )->styles() ) {
        d->m_rootDocument->addStyle( style );
    }

    foreach(const GeoDataStyleMap& map, static_cast<GeoDataDocument*>( document )->styleMaps() ) {
        d->m_rootDocument->addStyleMap( map );
    }
    
    emit( dataChanged() );
    
    return d->m_latestId;
}

bool MarbleGeoDataModel::removeGeoDataFile( unsigned long removeId )
{
    if(d->m_documents.contains( removeId ) ) {
        GeoDataDocument *doc = d->m_documents[ removeId ];
        
        // FIXME: here all features should be removed from the geodata model; this should probably work in the same way it does for the MarblePlacemarkModel
/*        foreach( const GeoDataFeature& feature, doc->features() ) {
            d->m_rootDocument->removeFeature( feature );
        }*/
        // get the styles and the stylemaps
        foreach(const GeoDataStyle& style, doc->styles() ) {
            d->m_rootDocument->removeStyle( style.styleId() );
        }

        foreach(const GeoDataStyleMap& map, doc->styleMaps() ) {
            d->m_rootDocument->removeStyleMap( map.styleId() );
        }
        delete doc;
        return true;
    }
    return false;
}

GeoDataDocument* MarbleGeoDataModel::geoDataRoot()
{
    return d->m_rootDocument;
}


#include "MarbleGeoDataModel.moc"
