//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmRelationManagerWidget_p.h"
#include "OsmRelationManagerWidget.h"

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "OsmPlacemarkData.h"
#include "MarbleDebug.h"

// Qt
#include <QTreeWidget>
#include <QMenu>
#include <QLineEdit>
#include <QList>

namespace Marble
{

OsmRelationManagerWidgetPrivate::OsmRelationManagerWidgetPrivate( OsmRelationManagerWidget* q_ )
    :q( q_ )
{
    // nothing to do
}

OsmRelationManagerWidgetPrivate::~OsmRelationManagerWidgetPrivate()
{
    // nothing to do
}

void OsmRelationManagerWidgetPrivate::init( QWidget* parent )
{
    setupUi( parent );
    populateRelationsList();
    m_relationDropMenu = new QMenu( m_addRelation );
    m_currentRelations->setRootIsDecorated( false );
    m_currentRelations->setEditTriggers( QTreeWidget::DoubleClicked );
    m_currentRelations->setContextMenuPolicy( Qt::CustomContextMenu );
    m_currentRelations->setMinimumWidth( m_currentRelations->columnCount() *
                                         m_currentRelations->columnWidth( 0 ) + 10 );

    m_addRelation->setMenu( m_relationDropMenu );
    populateDropMenu();

    QObject::connect( m_currentRelations, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
                      q, SLOT( handleDoubleClick(QTreeWidgetItem*,int) ) );
    QObject::connect( m_currentRelations, SIGNAL( customContextMenuRequested( QPoint ) ),
                      q, SLOT( handleRelationContextMenuRequest( QPoint ) ) );
    QObject::connect( m_relationDropMenu, SIGNAL( triggered( QAction* ) ),
                      q, SLOT( addRelation( QAction* ) ) );
    QObject::connect( m_currentRelations, SIGNAL( itemChanged(QTreeWidgetItem*,int) ),
                      q, SLOT( handleItemChange( QTreeWidgetItem*,int ) ) );

}

void OsmRelationManagerWidgetPrivate::populateRelationsList()
{
    m_currentRelations->clear();

    // This shouldn't happen
    if ( !m_allRelations ) {
        return;
    }

    if ( m_placemark->hasOsmData() ) {
        const OsmPlacemarkData &osmData = m_placemark->osmData();
        QHash< qint64, QString >::const_iterator it = osmData.relationReferencesBegin();
        QHash< qint64, QString >::const_iterator end = osmData.relationReferencesEnd();

        for ( ; it != end; ++it ) {

            if ( !m_allRelations->contains( it.key() ) ) {
                mDebug()<< QString( "Relation %1 is not loaded in the Annotate Plugin" ).arg( it.key() );
                continue;
            }

            const OsmPlacemarkData &relationData = m_allRelations->value( it.key() );

            QTreeWidgetItem *newItem = new QTreeWidgetItem();
            QString name = relationData.tagValue( "name" );
            QString type = relationData.tagValue( "type" );
            QString role = it.value();
            newItem->setText( Column::Name, name );
            newItem->setText( Column::Type, type );
            newItem->setText( Column::Role, role );
            newItem->setData( Column::Name, Qt::UserRole, relationData.id() );
            m_currentRelations->addTopLevelItem( newItem );

        }
    }
}

void OsmRelationManagerWidgetPrivate::populateDropMenu()
{
    m_relationDropMenu->clear();

    m_addRelation->setIcon( QIcon( ":marble/list-add.png" ) );

    // The new relation adder
    m_relationDropMenu->addAction( QObject::tr( "New Relation" ) );
    m_relationDropMenu->addSeparator();

    // This shouldn't happen
    Q_ASSERT( m_allRelations );

    // Suggesting existing relations
    foreach ( const OsmPlacemarkData &relationData, m_allRelations->values() ) {
        QString relationText = QString( "%1 (%2)" ).arg( relationData.tagValue( "name" ) ).arg( relationData.tagValue( "type" ) );

        // Don't suggest relations the placemark is already part of
        if ( m_placemark->hasOsmData() && m_placemark->osmData().containsRelation( relationData.id() ) ) {
            continue;
        }
        QAction *newAction = new QAction( m_relationDropMenu );
        newAction->setText( relationText );
        newAction->setData( relationData.id() );
        m_relationDropMenu->addAction( newAction );
    }
}

}
