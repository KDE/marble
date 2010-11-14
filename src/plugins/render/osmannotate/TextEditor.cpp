//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "TextEditor.h"

#include "MarbleDebug.h"

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QTextEdit>
#include <QtGui/QHBoxLayout>


namespace Marble
{

TextEditor::TextEditor() : QWidget()
{
    setCursor( Qt::ArrowCursor );

    m_buttonLayout = new QHBoxLayout;
    m_layout = new QVBoxLayout;

    /*
     *Note: these widgets do not need to be constructed with a parent
     *as adding them to a layout automatically sets the parent. If a
     *parent is already set it will mess up the layouts.
     */
    m_description = new QTextEdit;
    m_description->setMinimumHeight( 50 );
    m_description->setSizePolicy( QSizePolicy::Fixed,
                                  QSizePolicy::MinimumExpanding );
    m_description->viewport()->setSizePolicy( QSizePolicy::Fixed,
                                              QSizePolicy::MinimumExpanding );
    m_description->viewport()->setAutoFillBackground( true );
    QApplication::setPalette( QPalette() );
    m_description->setBackgroundRole( QPalette::Window );

    m_name = new QLineEdit;
    m_name->setText(tr("Placemark Name"));

    // setup the actions and create the buttons
    m_boldAction = new QAction( this );
    m_boldAction->setText(tr("Bold"));
    m_boldButton = new QToolButton;
    m_boldButton->setDefaultAction( m_boldAction );

    m_italicAction = new QAction( this );
    m_italicAction->setText( tr("Italic") );
    m_italicButton = new QToolButton;
    m_italicButton->setDefaultAction( m_italicAction );

    m_underLineAction = new QAction( this );
    m_underLineAction->setText( tr("Underline") );
    m_underLineButton = new QToolButton;
    m_underLineButton->setDefaultAction( m_underLineAction );

    //add them all to their correct places
    m_buttonLayout->addWidget( m_boldButton );
    m_buttonLayout->addWidget( m_italicButton );
    m_buttonLayout->addWidget( m_underLineButton );

    m_layout->addWidget( m_name );
    m_layout->addLayout( m_buttonLayout );
    m_layout->addWidget( m_description );

    setLayout( m_layout );
    setMaximumWidth( 250 );
    setMinimumHeight( 50 );
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
}

TextEditor::~TextEditor()
{

}

QString TextEditor::name() const
{
    return m_name->text();
}

void TextEditor::setName(const QString &name )
{
    m_name->setText( name );
}

QString TextEditor::description() const
{
    return m_description->toHtml();
}

void TextEditor::setDescription( const QString &description )
{
    m_description->setHtml( description );
}

}
