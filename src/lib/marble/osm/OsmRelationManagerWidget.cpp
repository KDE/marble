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
#include "OsmRelationManagerWidget.h"
#include "OsmRelationManagerWidget_p.h"
#include "ui_OsmRelationManagerWidget.h"

// Qt
#include <QWidget>
#include <QTreeWidget>
#include <QMenu>
#include <QPointer>
#include <QDebug>

// Marble
#include "GeoDataTypes.h"
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"
#include "osm/OsmRelationEditorDialog.h"

namespace Marble
{

OsmRelationManagerWidget::OsmRelationManagerWidget( GeoDataPlacemark *placemark,
                                                    const QHash<qint64, OsmPlacemarkData> *relations,
                                                    QWidget *parent )
    : QWidget( parent ),
      d( new OsmRelationManagerWidgetPrivate )
{
    d->m_placemark = placemark;
    d->m_allRelations = relations;
    d->setupUi( this );
    d->populateRelationsList();
    d->m_relationDropMenu = new QMenu( d->m_addRelation );
    d->m_currentRelations->setRootIsDecorated( false );
    d->m_currentRelations->setEditTriggers( QTreeWidget::DoubleClicked );
    d->m_currentRelations->setContextMenuPolicy( Qt::CustomContextMenu );
    d->m_currentRelations->setMinimumWidth( d->m_currentRelations->columnCount() * d->m_currentRelations->columnWidth( 0 ) + 10 );

    d->m_addRelation->setMenu( d->m_relationDropMenu );
    d->populateDropMenu();

    QObject::connect( d->m_currentRelations, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
                      this, SLOT( handleDoubleClick(QTreeWidgetItem*,int) ) );
    QObject::connect( d->m_currentRelations, SIGNAL( customContextMenuRequested( QPoint ) ),
                      this, SLOT( handleRelationContextMenuRequest( QPoint ) ) );
    QObject::connect( d->m_relationDropMenu, SIGNAL( triggered( QAction* ) ),
                      this, SLOT( addRelation( QAction* ) ) );
    QObject::connect( d->m_currentRelations, SIGNAL( itemChanged(QTreeWidgetItem*,int) ),
                      this, SLOT( handleItemChange( QTreeWidgetItem*,int ) ) );
}

OsmRelationManagerWidget::~OsmRelationManagerWidget()
{
    delete d;
}

void OsmRelationManagerWidget::addRelation( QAction *relationAction )
{
    // The QAction::text() adds a '&' for some reason
    QString relationText = relationAction->text().remove(QLatin1Char('&'));

    if ( relationText == tr( "New Relation" ) ) {
        OsmPlacemarkData relationData;
        QPointer<OsmRelationEditorDialog> relationEditor = new OsmRelationEditorDialog( &relationData );
        const int result = relationEditor->exec();
        delete relationEditor;
        if (result == QDialog::Rejected) {
            return;
        }

        QTreeWidgetItem *newRelationItem = new QTreeWidgetItem();
        newRelationItem->setText(Column::Name, relationData.tagValue(QStringLiteral("name")));
        newRelationItem->setText(Column::Type, relationData.tagValue(QStringLiteral("type")));
        newRelationItem->setData( Column::Name, Qt::UserRole, relationData.id() );
        d->m_currentRelations->addTopLevelItem( newRelationItem );

        // Make the user complete the role column
        newRelationItem->setFlags( newRelationItem->flags() | Qt::ItemIsEditable );
        d->m_currentRelations->editItem( newRelationItem, Column::Role );

        // This tells the annotate plugin to add the new relation to its list
        emit relationCreated( relationData );
    }
    else {
        qint64 id = relationAction->data().toLongLong();
        OsmPlacemarkData relationData = d->m_allRelations->value( id );
        QTreeWidgetItem *newRelationItem = new QTreeWidgetItem();
        newRelationItem->setText(Column::Name, relationData.tagValue(QStringLiteral("name")));
        newRelationItem->setText(Column::Type, relationData.tagValue(QStringLiteral("type")));
        newRelationItem->setData( Column::Name, Qt::UserRole, relationData.id() );
        d->m_currentRelations->addTopLevelItem( newRelationItem );

        // Make the user complete the role column
        newRelationItem->setFlags( newRelationItem->flags() | Qt::ItemIsEditable );
        d->m_currentRelations->editItem( newRelationItem, Column::Role );
    }
}

void OsmRelationManagerWidget::update()
{
    d->populateRelationsList();
    d->populateDropMenu();
}

void OsmRelationManagerWidget::handleDoubleClick( QTreeWidgetItem * item, int column )
{
    Qt::ItemFlags flags = item->flags();

    // Only the "role" column should be editable
    if ( column == Column::Role ) {
        item->setFlags( flags | Qt::ItemIsEditable );
    // If the double click didn't occur on the "role" column, and the item
    // is editable make it uneditable
    } else if ( flags & Qt::ItemIsEditable ) {
        item->setFlags( flags ^ Qt::ItemIsEditable );
    }
}

void OsmRelationManagerWidget::handleItemChange( QTreeWidgetItem *item, int column )
{
    // Only the role column should be editable
    if ( column != Column::Role ) {
        return;
    }

    QString role = item->text( Column::Role );
    qint64 id = item->data( Column::Name, Qt::UserRole ).toLongLong();

    d->m_placemark->osmData().addRelation( id, role );
    update();
}

void OsmRelationManagerWidget::handleRelationContextMenuRequest( const QPoint& point )
{
    QMenu relationEditMenu;
    relationEditMenu.addAction( tr( "Remove" ) );
    relationEditMenu.addAction( tr( "Edit" ) );
    QAction* selectedItem = relationEditMenu.exec( d->m_currentRelations->mapToGlobal( point ) );
    if ( selectedItem ) {

        QTreeWidgetItem *requestedItem = d->m_currentRelations->itemAt( point );
        qint64 id = requestedItem->data( Column::Name, Qt::UserRole ).toLongLong();

        if ( selectedItem->text() == tr( "Remove" ) ) {
            d->m_placemark->osmData().removeRelation( id );
            update();
        }
        else if ( selectedItem->text() == tr( "Edit" ) ) {
            OsmPlacemarkData relationData = d->m_allRelations->value( id );
            QPointer<OsmRelationEditorDialog> relationEditor = new OsmRelationEditorDialog( &relationData );
            const int result = relationEditor->exec();
            delete relationEditor;
            if (result == QDialog::Rejected) {
                return;
            }

            emit relationCreated( relationData );
            update();
        }
    }
}

}

#include "moc_OsmRelationManagerWidget.cpp"
