//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
// Copyright 2015      Constantin Mihalache <mihalache.c94@gmail.com>
//

//self
#include "FormattedTextWidget.h"
#include "ui_FormattedTextWidget.h"

//Qt
#include <QFileDialog>
#include <QColorDialog>
#include <QFontComboBox>
#include <QLineEdit>
#include <QPointer>

//Marble
#include "MarbleWidget.h"
#include "AddLinkDialog.h"

namespace Marble{

class Q_DECL_HIDDEN FormattedTextWidget::Private : public Ui::FormattedTextWidget
{
public:
    Private();
    ~Private();

    QColorDialog *m_textColorDialog;
};

FormattedTextWidget::Private::Private() :
    Ui::FormattedTextWidget(),
    m_textColorDialog( 0 )
{
    //nothing to do
}

FormattedTextWidget::Private::~Private()
{
    delete m_textColorDialog;
}

FormattedTextWidget::FormattedTextWidget( QWidget *parent ) :
    QWidget( parent ),
    d( new Private() )
{
    d->setupUi( this );

    d->m_formattedTextToolBar->insertSeparator( d->m_actionAddImage );
    QPixmap textColorPixmap(20, 20);
    textColorPixmap.fill( d->m_description->textCursor().charFormat().foreground().color() );
    d->m_actionColor->setIcon( textColorPixmap );
    d->m_textColorDialog = new QColorDialog( this );
    d->m_textColorDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_textColorDialog->setCurrentColor( d->m_description->textCursor().charFormat().foreground().color() );
    d->m_fontSize->setValidator( new QIntValidator( 1, 9000, this ) );
    int index = d->m_fontSize->findText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    if( index != -1 ) {
        d->m_fontSize->setCurrentIndex( index );
    } else {
        d->m_fontSize->lineEdit()->setText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    }
    connect( d->m_actionColor, SIGNAL(triggered()), d->m_textColorDialog, SLOT(exec()) );
    connect( d->m_textColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(setTextCursorColor(QColor)) );
    connect( d->m_isFormattedTextMode, SIGNAL(toggled(bool)), this, SLOT(toggleDescriptionEditMode(bool)) );
    connect( d->m_fontFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(setTextCursorFont(QFont)) );
    connect( d->m_fontSize, SIGNAL(editTextChanged(QString)), this, SLOT(setTextCursorFontSize(QString)) );
    connect( d->m_actionBold, SIGNAL(toggled(bool)), this, SLOT(setTextCursorBold(bool)) );
    connect( d->m_actionItalics, SIGNAL(toggled(bool)), this, SLOT(setTextCursorItalic(bool)) );
    connect( d->m_actionUnderlined, SIGNAL(toggled(bool)), this, SLOT(setTextCursorUnderlined(bool)) );
    connect( d->m_actionAddImage, SIGNAL(triggered()), this, SLOT(addImageToDescription()) );
    connect( d->m_actionAddLink, SIGNAL(triggered()), this, SLOT(addLinkToDescription()) );
    connect( d->m_description, SIGNAL(cursorPositionChanged()), this, SLOT(updateDescriptionEditButtons()) );
}

FormattedTextWidget::~FormattedTextWidget()
{
    delete d;
}

void FormattedTextWidget::setText( const QString &text )
{
    d->m_description->setHtml( text );
}

const QString FormattedTextWidget::text()
{
    return d->m_description->toHtml();
}

void FormattedTextWidget::toggleDescriptionEditMode( bool isFormattedTextMode )
{
    d->m_formattedTextToolBar->setVisible( isFormattedTextMode );
    d->m_fontSize->setVisible( isFormattedTextMode );
    d->m_fontFamily->setVisible( isFormattedTextMode );
    if( isFormattedTextMode ) {
        d->m_description->setHtml( d->m_description->toPlainText() );
    } else {
        QTextCursor cursor = d->m_description->textCursor();
        QTextCharFormat format;
        format.setFont( QFont() );
        format.setFontWeight( QFont::Normal );
        format.setFontItalic( false );
        format.setFontUnderline( false );
        format.clearForeground();
        cursor.setCharFormat( format );
        d->m_description->setTextCursor( cursor );
        d->m_description->setPlainText( d->m_description->toHtml() );
    }
}

void FormattedTextWidget::setTextCursorBold( bool bold )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontWeight( bold ? QFont::Bold : QFont::Normal );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void FormattedTextWidget::setTextCursorItalic( bool italic )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontItalic( italic );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void FormattedTextWidget::setTextCursorUnderlined( bool underlined )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontUnderline( underlined );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void FormattedTextWidget::setTextCursorColor( const QColor &color )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    QBrush brush( color );
    format.setForeground( brush );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
    QPixmap textColorPixmap(22, 22);
    textColorPixmap.fill( format.foreground().color() );
    d->m_actionColor->setIcon( QIcon( textColorPixmap ) );
    d->m_textColorDialog->setCurrentColor( format.foreground().color() );
}

void FormattedTextWidget::setTextCursorFont( const QFont &font )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontFamily( font.family() );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void FormattedTextWidget::setTextCursorFontSize( const QString &fontSize )
{
    bool ok = false;
    int size = fontSize.toInt( &ok );
    if( ok ) {
        QTextCursor cursor = d->m_description->textCursor();
        QTextCharFormat format;
        format.setFontPointSize( size );
        cursor.mergeCharFormat( format );
        d->m_description->setTextCursor( cursor );
    }
}

void FormattedTextWidget::addImageToDescription()
{
    QString filename = QFileDialog::getOpenFileName( this, tr( "Choose image" ), tr( "All Supported Files (*.png *.jpg *.jpeg)" )  );
    QImage image( filename );
    if( !image.isNull() ) {
        QTextCursor cursor = d->m_description->textCursor();
        cursor.insertImage( image, filename );
    }
}

void FormattedTextWidget::addLinkToDescription()
{
    QPointer<AddLinkDialog> dialog = new AddLinkDialog( this );
    if( dialog->exec() ) {
        QTextCharFormat oldFormat = d->m_description->textCursor().charFormat();
        QTextCharFormat linkFormat = oldFormat;
        linkFormat.setAnchor( true );
        linkFormat.setFontUnderline( true );
        linkFormat.setForeground( QApplication::palette().link() );
        linkFormat.setAnchorHref( dialog->url() );
        d->m_description->textCursor().insertText( dialog->name(), linkFormat );
        QTextCursor cursor =  d->m_description->textCursor();
        cursor.setCharFormat( oldFormat );
        d->m_description->setTextCursor( cursor );
        d->m_description->textCursor().insertText( " " );
    }
}

void FormattedTextWidget::updateDescriptionEditButtons()
{
    disconnect( d->m_actionBold, SIGNAL(toggled(bool)), this, SLOT(setTextCursorBold(bool)) );
    disconnect( d->m_actionItalics, SIGNAL(toggled(bool)), this, SLOT(setTextCursorItalic(bool)) );
    disconnect( d->m_actionUnderlined, SIGNAL(toggled(bool)), this, SLOT(setTextCursorUnderlined(bool)) );
    disconnect( d->m_fontFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(setTextCursorFont(QFont)) );
    disconnect( d->m_fontSize, SIGNAL(editTextChanged(QString)), this, SLOT(setTextCursorFontSize(QString)) );

    QTextCharFormat format = d->m_description->textCursor().charFormat();

    d->m_fontFamily->setCurrentFont( format.font() );

    if( format.fontWeight() == QFont::Bold ) {
        d->m_actionBold->setChecked( true );
    } else if ( format.fontWeight() == QFont::Normal ) {
        d->m_actionBold->setChecked( false );
    }
    d->m_actionItalics->setChecked( format.fontItalic() );
    d->m_actionUnderlined->setChecked( format.fontUnderline() );

    QPixmap textColorPixmap(22, 22);
    textColorPixmap.fill( format.foreground().color() );
    d->m_actionColor->setIcon( QIcon( textColorPixmap ) );
    d->m_textColorDialog->setCurrentColor( format.foreground().color() );

    int index = d->m_fontSize->findText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    if( index != -1 ) {
        d->m_fontSize->setCurrentIndex( index );
    } else {
        d->m_fontSize->lineEdit()->setText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    }
    connect( d->m_actionBold, SIGNAL(toggled(bool)), this, SLOT(setTextCursorBold(bool)) );
    connect( d->m_actionItalics, SIGNAL(toggled(bool)), this, SLOT(setTextCursorItalic(bool)) );
    connect( d->m_actionUnderlined, SIGNAL(toggled(bool)), this, SLOT(setTextCursorUnderlined(bool)) );
    connect( d->m_fontFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(setTextCursorFont(QFont)) );
    connect( d->m_fontSize, SIGNAL(editTextChanged(QString)), this, SLOT(setTextCursorFontSize(QString)) );
}

void FormattedTextWidget::setReadOnly( bool state )
{
    d->m_description->setReadOnly( state );
    d->m_formattedTextToolBar->setDisabled( state );
    d->m_fontFamily->setDisabled( state );
    d->m_fontSize->setDisabled( state );
    d->m_actionColor->setDisabled( state );
}

}

#include "moc_FormattedTextWidget.cpp"
