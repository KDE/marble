#include "TextEditor.h"

#include <QtGui/QAction>
#include <QtGui/QToolButton>
#include <QtGui/QTextEdit>
#include <QtGui/QHBoxLayout>


namespace Marble {

TextEditor::TextEditor()
{
    m_buttonLayout = new QHBoxLayout( this );
    m_layout = new QVBoxLayout( this );

    m_textEditor = new QTextEdit( this );

    // setup the actions and create the buttons
    m_boldAction = new QAction( this );
    m_boldAction->setText("Bold");
    m_boldButton = new QToolButton( this );
    m_boldButton->setDefaultAction( m_boldAction );

    m_italicAction = new QAction( this );
    m_italicAction->setText( "Italic" );
    m_italicButton = new QToolButton( this );
    m_italicButton->setDefaultAction( m_italicAction );

    m_underLineAction = new QAction( this );
    m_underLineAction->setText( "UnderLine" );
    m_underLineButton = new QToolButton( this );
    m_underLineButton->setDefaultAction( m_underLineAction );

    //add them all to their correct places
    m_buttonLayout->addWidget( m_boldButton );
    m_buttonLayout->addWidget( m_italicButton );
    m_buttonLayout->addWidget( m_underLineButton );

    m_layout->addLayout( m_buttonLayout );
    m_layout->addWidget( m_textEditor );



    setLayout( m_layout );
}

TextEditor::~TextEditor()
{

}

}
