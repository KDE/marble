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

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>

#include "SoundCueEditWidget.h"
#include "MarbleWidget.h"
#include "geodata/data/GeoDataSoundCue.h"
#include "GeoDataTypes.h"
#include "MarblePlacemarkModel.h"

namespace Marble {

SoundCueEditWidget::SoundCueEditWidget( const QModelIndex &index, QWidget *parent ) :
    QWidget( parent ),
    m_index( index ),
    m_lineEdit( new QLineEdit )
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing( 5 );

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap( QPixmap( ":/marble/playback-play.png" ) );
    layout->addWidget( iconLabel );

    m_lineEdit->setPlaceholderText( "Audio location" );
    layout->addWidget( m_lineEdit );

    QToolButton *button = new QToolButton;
    button->setIcon( QIcon( ":/marble/document-save.png" ) );
    connect(button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget( button );

    setLayout( layout );
}

void SoundCueEditWidget::save()
{
    soundCueElement()->setHref( m_lineEdit->text() );
    emit editingDone(m_index);
}

GeoDataSoundCue* SoundCueEditWidget::soundCueElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(m_index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    Q_ASSERT( object );
    Q_ASSERT( object->nodeType() == GeoDataTypes::GeoDataSoundCueType );
    return static_cast<GeoDataSoundCue*>( object );
}

} // namespace Marble

#include "SoundCueEditWidget.moc"
