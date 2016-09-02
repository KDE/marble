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
#include <QFileDialog>

#include "SoundCueEditWidget.h"
#include "MarbleWidget.h"
#include "geodata/data/GeoDataSoundCue.h"
#include "GeoDataTypes.h"
#include "MarblePlacemarkModel.h"

namespace Marble {

SoundCueEditWidget::SoundCueEditWidget( const QModelIndex &index, QWidget *parent ) :
    QWidget( parent ),
    m_index( index ),
    m_lineEdit( new QLineEdit ),
    m_button( new QToolButton ),
    m_button2( new QToolButton )
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing( 5 );

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/playback-play.png")));
    layout->addWidget( iconLabel );

    m_lineEdit->setPlaceholderText( "Audio location" );
    m_lineEdit->setText( soundCueElement()->href() );
    layout->addWidget( m_lineEdit );

    m_button2->setIcon(QIcon(QStringLiteral(":/marble/document-open.png")));
    connect(m_button2, SIGNAL(clicked()), this, SLOT(open()));
    layout->addWidget( m_button2 );

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget( m_button );

    setLayout( layout );
}

bool SoundCueEditWidget::editable() const
{
    return m_button->isEnabled();
}

void SoundCueEditWidget::setEditable( bool editable )
{
    m_button->setEnabled( editable );
}

void SoundCueEditWidget::save()
{
    soundCueElement()->setHref( m_lineEdit->text() );
    emit editingDone(m_index);
}
void SoundCueEditWidget::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select sound files..."), QDir::homePath(), tr("Supported Sound Files (*.mp3 *.ogg *.wav)"));
    m_lineEdit->setText(fileName);
    soundCueElement()->setHref( m_lineEdit->text() );
}

GeoDataSoundCue* SoundCueEditWidget::soundCueElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(m_index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    Q_ASSERT( object );
    Q_ASSERT( object->nodeType() == GeoDataTypes::GeoDataSoundCueType );
    return static_cast<GeoDataSoundCue*>( object );
}

} // namespace Marble

#include "moc_SoundCueEditWidget.cpp"
