//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// self
#include "OsmRelationEditorDialog.h"

// Marble
#include "OsmTagEditorWidget.h"
#include "OsmPlacemarkData.h"
#include "OsmObjectManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"

// Qt
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

namespace Marble {


OsmRelationEditorDialog::OsmRelationEditorDialog( OsmPlacemarkData *relationData, QWidget *parent ) :
    QDialog( parent )
{
    m_relationData = relationData;
    QVBoxLayout *layout = new QVBoxLayout( this );

    // Name input area
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel( tr( "Name" ), this );
    m_nameLineEdit = new QLineEdit( this );
    m_nameLineEdit->setText(relationData->tagValue(QStringLiteral("name")));
    nameLayout->addWidget( nameLabel );
    nameLayout->addWidget( m_nameLineEdit );
    layout->addLayout( nameLayout );

    // Tag editor area
    // A dummy placemark is needed because the OsmTagEditorWidget works with placemarks
    m_dummyPlacemark = new GeoDataPlacemark();
    // "osmRelaation=yes" entry is added to its ExtendedData to let the widget know
    // its special relation status
    GeoDataExtendedData extendedData;
    extendedData.addValue(GeoDataData(QStringLiteral("osmRelation"), QStringLiteral("yes")));
    m_dummyPlacemark->setExtendedData( extendedData );
    m_dummyPlacemark->setOsmData( *m_relationData );
    OsmObjectManager::initializeOsmData( m_dummyPlacemark );
    m_tagEditor = new OsmTagEditorWidget( m_dummyPlacemark, this );
    layout->addWidget( m_tagEditor );

    // Button box area
    m_buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    layout->addWidget( m_buttonBox );

    QObject::connect( m_buttonBox, SIGNAL( accepted() ),
                       this, SLOT( checkFields() ) );
    connect(m_buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

OsmRelationEditorDialog::~OsmRelationEditorDialog()
{
    delete m_dummyPlacemark;
}

void OsmRelationEditorDialog::finish()
{
    // Updating the relation data with the edited one
    m_dummyPlacemark->osmData().addTag(QStringLiteral("name"), m_nameLineEdit->text());
    *m_relationData = m_dummyPlacemark->osmData();
    accept();
}

void OsmRelationEditorDialog::checkFields()
{
    if ( m_nameLineEdit->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No name specified" ),
                              tr( "Please specify a name for this relation." ) );
    }
    else if (!m_dummyPlacemark->osmData().containsTagKey(QStringLiteral("type"))) {
        QMessageBox::warning( this,
                              tr( "No type tag specified" ),
                              tr( "Please add a type tag for this relation." ) );
    }
    else {
        finish();
    }
}

}

#include "moc_OsmRelationEditorDialog.cpp"

