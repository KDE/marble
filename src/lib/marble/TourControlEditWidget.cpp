//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// Copyright 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QRadioButton>

#include "TourControlEditWidget.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataTypes.h"
#include "GeoDataTourControl.h"

namespace Marble
{

TourControlEditWidget::TourControlEditWidget( const QModelIndex &index, QWidget *parent ) :
    QWidget( parent ),
    m_index( index ),
    m_radio_play( new QRadioButton ),
    m_radio_pause( new QRadioButton ),
    m_button( new QToolButton )
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing( 5 );

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/media-playback-pause.png")));
    layout->addWidget( iconLabel );

    layout->addWidget( m_radio_play );
    m_radio_play->setText( tr( "Play" ) );

    layout->addWidget( m_radio_pause );
    m_radio_pause->setText( tr( "Pause" ) );

    if( tourControlElement()->playMode() == GeoDataTourControl::Play ){
        m_radio_play->setChecked( true );
    }else{
        m_radio_pause->setChecked( true );
    }

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget( m_button );

    setLayout( layout );
}

bool TourControlEditWidget::editable() const
{
    return m_button->isEnabled();
}

void TourControlEditWidget::setEditable( bool editable )
{
    m_button->setEnabled( editable );
}

void TourControlEditWidget::save()
{
    if ( m_radio_play->isChecked() ) {
        tourControlElement()->setPlayMode( GeoDataTourControl::Play );
    } else {
        tourControlElement()->setPlayMode( GeoDataTourControl::Pause );
    }
    emit editingDone(m_index);
}

GeoDataTourControl* TourControlEditWidget::tourControlElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(m_index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    Q_ASSERT( object );
    Q_ASSERT( object->nodeType() == GeoDataTypes::GeoDataTourControlType );
    return static_cast<GeoDataTourControl*>( object );
}

} // namespace Marble

#include "moc_TourControlEditWidget.cpp"
