//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "ConflictDialog.h"

#include "MergeItem.h"

#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVariant>

namespace Marble {

ConflictDialog::ConflictDialog( QWidget *parent ) :
    QDialog( parent ),
    m_mergeItem( 0 ),
    m_box( 0 ),
    m_resolveAction(ConflictDialog::AskUser)
{
    // nothing to do
}

void ConflictDialog::setMergeItem( MergeItem *item )
{
    m_mergeItem = item;
}

void ConflictDialog::stopAutoResolve()
{
    m_resolveAction = ConflictDialog::AskUser;
}

void ConflictDialog::open()
{
    if( m_mergeItem == 0 ) {
        return;
    }

    switch( m_resolveAction ) {
    case ConflictDialog::AskUser:
        prepareLayout();
        QDialog::open();
        break;
    case ConflictDialog::PreferLocal:
        m_mergeItem->setResolution( MergeItem::A );
        emit resolveConflict( m_mergeItem );
        break;
    case ConflictDialog::PreferCloud:
        m_mergeItem->setResolution( MergeItem::B );
        emit resolveConflict( m_mergeItem );
        break;
    }
}

void ConflictDialog::resolveConflict( QAbstractButton *button )
{
    accept();

    QDialogButtonBox::StandardButton standardButton = m_box->standardButton( button );
    switch(standardButton) {
    case QDialogButtonBox::NoButton:
    {
        int actionRole = button->property( "ActionRole" ).toInt();
        switch( actionRole ) {
        case ConflictDialog::Local:
            m_mergeItem->setResolution( MergeItem::A );
            emit resolveConflict( m_mergeItem );
            break;
        case ConflictDialog::Cloud:
            m_mergeItem->setResolution( MergeItem::B );
            emit resolveConflict( m_mergeItem );
            break;
        case ConflictDialog::AllLocal:
            m_mergeItem->setResolution( MergeItem::A );
            m_resolveAction = ConflictDialog::PreferLocal;
            emit resolveConflict( m_mergeItem );
            break;
        case ConflictDialog::AllCloud:
            m_mergeItem->setResolution( MergeItem::B );
            m_resolveAction = ConflictDialog::PreferCloud;
            emit resolveConflict( m_mergeItem );
            break;
        default:
            break;
        }
    }

    default:
        break;
    }
}

void ConflictDialog::prepareLayout()
{
    delete layout();
    qDeleteAll( children() );
    m_box = new QDialogButtonBox( QDialogButtonBox::Cancel );

    QPushButton *localButton = new QPushButton( tr( "Use local" ) );
    QPushButton *cloudButton = new QPushButton( tr( "Use cloud" ) );
    QPushButton *allLocalButton = new QPushButton( tr( "Always use local" ) );
    QPushButton *allCloudButton = new QPushButton( tr( "Always use cloud" ) );

    localButton->setDefault( true );
    localButton->setProperty( "ActionRole", ConflictDialog::Local );
    cloudButton->setProperty( "ActionRole", ConflictDialog::Cloud );
    allLocalButton->setProperty( "ActionRole", ConflictDialog::AllLocal );
    allCloudButton->setProperty( "ActionRole", ConflictDialog::AllCloud );

    m_box->addButton( localButton, QDialogButtonBox::ActionRole );
    m_box->addButton( cloudButton, QDialogButtonBox::ActionRole );
    m_box->addButton( allLocalButton, QDialogButtonBox::ActionRole );
    m_box->addButton( allCloudButton, QDialogButtonBox::ActionRole );

    QVBoxLayout *leftLayout = new QVBoxLayout();
    QString localHeaderText = tr( "Local placemark" );
    QString localDetailText = tr( "Path: %0 <br /> Name: %1 <br /> Description: %2 <br /> Status: %3" )
            .arg( m_mergeItem->pathA(), m_mergeItem->nameA(), m_mergeItem->descriptionA() );
    switch( m_mergeItem->actionA() ) {
    case MergeItem::Changed:
        localDetailText = localDetailText.arg( tr( "Changed" ) );
        break;
    case MergeItem::Deleted:
        localDetailText = localDetailText.arg( tr( "Deleted" ) );
        break;
    default:
        break;
    }

    QLabel *localHeaderLabel = new QLabel( localHeaderText );
    QLabel *localDetailLabel = new QLabel( localDetailText );
    leftLayout->addWidget( localHeaderLabel );
    leftLayout->addWidget( localDetailLabel );

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QString cloudHeaderText = tr( "Cloud placemark" );
    QString cloudDetailText = tr( "Path: %0 <br /> Name: %1 <br /> Description: %2 <br /> Status: %3" )
            .arg( m_mergeItem->pathB(), m_mergeItem->nameB(), m_mergeItem->descriptionB() );
    switch( m_mergeItem->actionB() ) {
    case MergeItem::Changed:
        cloudDetailText = cloudDetailText.arg( tr( "Changed" ) );
        break;
    case MergeItem::Deleted:
        cloudDetailText = cloudDetailText.arg( tr( "Deleted" ) );
        break;
    default:
        break;
    }
    QLabel *cloudHeaderLabel = new QLabel( cloudHeaderText );
    QLabel *cloudDetailLabel = new QLabel( cloudDetailText );
    rightLayout->addWidget( cloudHeaderLabel );
    rightLayout->addWidget( cloudDetailLabel );

    QHBoxLayout *detailLayout = new QHBoxLayout();
    detailLayout->addLayout( leftLayout );
    detailLayout->addLayout( rightLayout );

    QLabel *descriptionLabel = new QLabel();
    QString descriptionText = tr( "A bookmark on this device conflicts " \
                                  "with a cloud bookmark. Which one do " \
                                  "you want to keep?" );
    descriptionLabel->setText( descriptionText );

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget( descriptionLabel );
    mainLayout->addLayout( detailLayout );
    mainLayout->addWidget( m_box );

    setLayout( mainLayout );
    setWindowTitle( tr( "Synchronization Conflict" ) );

    connect( m_box, SIGNAL(clicked(QAbstractButton*)),
             this, SLOT(resolveConflict(QAbstractButton*)) );
}

}

#include "moc_ConflictDialog.cpp"
