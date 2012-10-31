//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Anton Chernov <chernov.anton.mail@gmail.com>
// Copyright 2012      "LOTES TM" LLC <lotes.sis@gmail.com>
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "DeclarativeDataPlugin.h"
#include "DeclarativeDataPluginModel.h"
#include "DeclarativeDataPluginItem.h"
#include "MarbleDeclarativeWidget.h"

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtScript/QScriptValue>

using namespace Marble;

class DeclarativeDataPluginPrivate {
public:
    DeclarativeDataPlugin* q;
    QString m_planet;
    QString m_name;
    QString m_nameId;
    QString m_version;
    QString m_guiString;
    QString m_copyrightYears;
    QString m_description;
    QList<Marble::PluginAuthor> m_authors;
    QString m_aboutText;
    bool m_isInitialized;
    QList<AbstractDataPluginItem *> m_items;
    QList<DeclarativeDataPluginModel*> m_modelInstances;
    QDeclarativeComponent* m_delegate;
    QVariant m_model;
    static int m_counter;

    DeclarativeDataPluginPrivate( DeclarativeDataPlugin* q );

    void parseChunk( DeclarativeDataPluginItem * item, GeoDataCoordinates &coordinates, const QString &key, const QVariant &value );

    void addItem( DeclarativeDataPluginItem* item, const GeoDataCoordinates &coordinates );

    void parseListModel( QAbstractListModel* listModel );

    void parseObject( QObject* object );
};

int DeclarativeDataPluginPrivate::m_counter = 0;

DeclarativeDataPluginPrivate::DeclarativeDataPluginPrivate( DeclarativeDataPlugin* parent ) :
    q( parent ), m_planet( "earth"), m_isInitialized( false ), m_delegate( 0 )
{
    ++m_counter;
}

void DeclarativeDataPluginPrivate::parseChunk( DeclarativeDataPluginItem *item, GeoDataCoordinates &coordinates, const QString &key, const QVariant &value )
{
    if( key == "lat" || key == "latitude" ) {
        coordinates.setLatitude( value.toDouble(), GeoDataCoordinates::Degree );
    } else if( key == "lon" || key == "longitude" ) {
        coordinates.setLongitude( value.toDouble(), GeoDataCoordinates::Degree );
    } else if( key == "alt" || key == "altitude" ) {
        coordinates.setAltitude( value.toDouble() );
    } else {
        item->setProperty( key.toAscii(), value );
    }
}

void DeclarativeDataPluginPrivate::addItem( DeclarativeDataPluginItem *item, const GeoDataCoordinates &coordinates )
{
    if ( coordinates.isValid() ) {
        item->setCoordinate( coordinates );
        item->setTarget( m_planet );
        QVariant const idValue = item->property( "identifier" );
        if ( idValue.isValid() && !idValue.toString().isEmpty() ) {
            item->setId( idValue.toString() );
        } else {
            item->setId( coordinates.toString() ) ;
        }
        m_items.append( item );
    } else {
        delete item;
    }
}

void DeclarativeDataPluginPrivate::parseListModel( QAbstractListModel *listModel )
{
    QHash< int, QByteArray > roles = listModel->roleNames();
    for( int i = 0; i < listModel->rowCount(); ++i ) {
        GeoDataCoordinates coordinates;
        QMap< int, QVariant > const itemData = listModel->itemData( listModel->index( i ) );
        QHash< int, QByteArray >::const_iterator it = roles.constBegin();
        DeclarativeDataPluginItem * item = new DeclarativeDataPluginItem( q );
        for ( ; it != roles.constEnd(); ++it ) {
            parseChunk( item, coordinates, it.value(), itemData.value( it.key() ) );
        }

        addItem( item, coordinates );
    }
}

void DeclarativeDataPluginPrivate::parseObject( QObject *object )
{
    int count;
    QMetaObject const * meta = object->metaObject();
    for( int i = 0; i < meta->propertyCount(); ++i ) {
        if( qstrcmp( meta->property(i).name(), "count" ) == 0 ) {
            count = meta->property(i).read( object ).toInt();
        }
    }

    for( int i = 0; i < meta->methodCount(); ++i ) {
        if( qstrcmp( meta->method(i).signature(), "get(int)" ) == 0 ) {
            for( int j=0; j < count; ++j ) {
                QScriptValue value;
                meta->method(i).invoke( object, Qt::AutoConnection, Q_RETURN_ARG( QScriptValue , value), Q_ARG( int, j ) );
                QObject * propertyObject = value.toQObject();
                GeoDataCoordinates coordinates;
                DeclarativeDataPluginItem * item = new DeclarativeDataPluginItem( q );
                if ( propertyObject ) {
                    for( int k = 0; k < propertyObject->metaObject()->propertyCount(); ++k ) {
                        QString const propertyName = propertyObject->metaObject()->property( k ).name();
                        QVariant const value = propertyObject->metaObject()->property( k ).read( propertyObject );
                        parseChunk( item, coordinates, propertyName, value );
                    }
                } else {
                    QScriptValueIterator it( value );
                    while ( it.hasNext() ) {
                        it.next();
                        parseChunk( item, coordinates, it.name(), it.value().toVariant() );
                    }
                }
                addItem( item, coordinates );
            }
        }
    }
}

Marble::RenderPlugin *DeclarativeDataPlugin::newInstance(const Marble::MarbleModel *marbleModel) const
{
    DeclarativeDataPlugin* instance = new DeclarativeDataPlugin( marbleModel );
    instance->d->m_planet = d->m_planet;
    instance->d->m_name = d->m_name;
    instance->d->m_nameId = d->m_nameId;
    instance->d->m_version = d->m_version;
    instance->d->m_guiString = d->m_guiString;
    instance->d->m_copyrightYears = d->m_copyrightYears;
    instance->d->m_description = d->m_description;
    instance->d->m_authors = d->m_authors;
    instance->d->m_aboutText = d->m_aboutText;
    instance->d->m_isInitialized = d->m_isInitialized;
    instance->d->m_items = d->m_items;
    instance->d->m_delegate = d->m_delegate;
    instance->d->m_model = d->m_model;
    instance->d->m_counter = d->m_counter;

    DeclarativeDataPluginModel* dataModel = new DeclarativeDataPluginModel( marbleModel->pluginManager() );
    dataModel->addItemsToList( d->m_items );
    instance->setModel( dataModel );
    connect( dataModel, SIGNAL( dataRequest( qreal, qreal, qreal, qreal ) ), this, SIGNAL( dataRequest( qreal, qreal, qreal, qreal ) ) );
    d->m_modelInstances << dataModel;
    return instance;
}

DeclarativeDataPlugin::DeclarativeDataPlugin( const Marble::MarbleModel *marbleModel )
    : AbstractDataPlugin( marbleModel ), d( new DeclarativeDataPluginPrivate( this ) )
{
    setEnabled( true );
    setVisible( true );
}

DeclarativeDataPlugin::~DeclarativeDataPlugin()
{
    delete d;
}

QString DeclarativeDataPlugin::planet() const
{
    return d->m_planet;
}

void DeclarativeDataPlugin::setPlanet( const QString &planet )
{
    if ( d->m_planet != planet ) {
        d->m_planet = planet;
        emit planetChanged();
    }
}

QString DeclarativeDataPlugin::name() const
{
    return d->m_name.isEmpty() ? "Anonymous DeclarativeDataPlugin" : d->m_name;
}

QString DeclarativeDataPlugin::guiString() const
{
    return d->m_guiString.isEmpty() ? name() : d->m_guiString;
}

QString DeclarativeDataPlugin::nameId() const
{
    return d->m_nameId.isEmpty() ? QString( "DeclarativeDataPlugin_%1" ).arg( d->m_counter ) : d->m_nameId;
}

QString DeclarativeDataPlugin::version() const
{
    return d->m_version.isEmpty() ? "1.0" : d->m_version;
}

QString DeclarativeDataPlugin::description() const
{
    return d->m_description;
}

QString DeclarativeDataPlugin::copyrightYears() const
{
    return d->m_copyrightYears;
}

QList<PluginAuthor> DeclarativeDataPlugin::pluginAuthors() const
{
    return d->m_authors;
}

QStringList DeclarativeDataPlugin::authors() const
{
    QStringList authors;
    foreach( const PluginAuthor& author, d->m_authors ) {
        authors<< author.name << author.email;
    }
    return authors;
}

QString DeclarativeDataPlugin::aboutDataText() const
{
    return d->m_aboutText;
}

QIcon DeclarativeDataPlugin::icon() const
{
    return QIcon();
}

void DeclarativeDataPlugin::setName( const QString & name )
{
    if( d->m_name != name ) {
        d->m_name = name;
        emit nameChanged();
    }
}

void DeclarativeDataPlugin::setGuiString( const QString & guiString )
{
    if( d->m_guiString != guiString ) {
        d->m_guiString = guiString;
        emit guiStringChanged();
    }
}

void DeclarativeDataPlugin::setNameId( const QString & nameId )
{
    if( d->m_nameId != nameId ) {
        d->m_nameId = nameId;
        emit nameIdChanged();
    }
}

void DeclarativeDataPlugin::setVersion( const QString & version )
{
    if( d->m_version != version ) {
        d->m_version = version;
        emit versionChanged();
    }
}

void DeclarativeDataPlugin::setCopyrightYears( const QString & copyrightYears )
{
    if( d->m_copyrightYears != copyrightYears ) {
        d->m_copyrightYears = copyrightYears;
        emit copyrightYearsChanged();
    }
}

void DeclarativeDataPlugin::setDescription( const QString description )
{
    if( d->m_description != description ) {
        d->m_description = description;
        emit descriptionChanged();
    }
}

void DeclarativeDataPlugin::setAuthors( const QStringList & pluginAuthors )
{
    if( pluginAuthors.size() % 2 == 0 ) {
        QStringList::const_iterator it = pluginAuthors.constBegin();
        while (  it != pluginAuthors.constEnd() ) {
            QString name = *(++it);
            QString email = *(++it);;
            d->m_authors.append( PluginAuthor( name, email) );
        }
        emit authorsChanged();
    }
}

void DeclarativeDataPlugin::setAboutDataText( const QString & aboutDataText )
{
    if( d->m_aboutText != aboutDataText ) {
        d->m_aboutText = aboutDataText;
        emit aboutDataTextChanged();
    }
}

QDeclarativeComponent *DeclarativeDataPlugin::delegate()
{
    return d->m_delegate;
}

void DeclarativeDataPlugin::setDelegate( QDeclarativeComponent *delegate )
{
    if ( delegate != d->m_delegate ) {
        d->m_delegate = delegate;
        emit delegateChanged();
    }
}

void DeclarativeDataPlugin::initialize()
{
    if( !model() ) {
        setModel( new DeclarativeDataPluginModel( pluginManager(), this ) );
    }
    d->m_isInitialized = true;
}

bool DeclarativeDataPlugin::isInitialized() const
{
    return d->m_isInitialized;
}

void DeclarativeDataPlugin::setDeclarativeModel( const QVariant &model )
{
    d->m_model = model;
    d->m_items.clear();

    QObject* object = qVariantValue<QObject*>( model );
    if( qobject_cast< QAbstractListModel* >( object ) ) {
        d->parseListModel( qobject_cast< QAbstractListModel *>( object ) );
    } else {
        d->parseObject( object );
    }

    /** @todo: Listen for and reflect changes to the items in the model */

    foreach( DeclarativeDataPluginModel* model, d->m_modelInstances ) {
        model->addItemsToList( d->m_items );
    }

    emit declarativeModelChanged();
}

QVariant DeclarativeDataPlugin::declarativeModel()
{
    return d->m_model;
}

#include "DeclarativeDataPlugin.moc"
