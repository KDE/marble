//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

// Self
#include "EditPolygonDialog.h"
#include "ui_EditPolygonDialog.h"

// Qt
#include <QColorDialog>

// Marble
#include "GeoDataStyle.h"


namespace Marble {

class EditPolygonDialog::Private : public Ui::UiEditPolygonDialog
{
public:
    Private( GeoDataPlacemark *placemark );
    ~Private();

    GeoDataPlacemark *m_placemark;

    QColorDialog *m_linesDialog;
    QColorDialog *m_polyDialog;
};

EditPolygonDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditPolygonDialog(),
    m_placemark( placemark ),
    m_linesDialog( 0 ),
    m_polyDialog( 0 )
{
    // nothing to do
}

EditPolygonDialog::Private::~Private()
{

}

EditPolygonDialog::EditPolygonDialog( GeoDataPlacemark *placemark, QWidget *parent ) :
    QDialog( parent ),
    d( new Private( placemark ) )
{
    d->setupUi( this );

    d->m_name->setText( placemark->name() );
    d->m_description->setText( placemark->description() );
    d->m_linesWidth->setRange( 0.1, 5.0 );

    // Get the current style properties.
    const GeoDataLineStyle lineStyle = placemark->style()->lineStyle();
    const GeoDataPolyStyle polyStyle = placemark->style()->polyStyle();

    // Adjust the "Filled"/"Not Filled" option according to its current fill.
    d->m_linesWidth->setValue( lineStyle.width() );
    if ( polyStyle.fill() ) {
        d->m_filledColor->setCurrentIndex( 0 );
    } else {
        d->m_filledColor->setCurrentIndex( 1 );
    }

    // Adjust the color buttons' icons to the current lines and polygon colors.
    QPixmap linesPixmap( d->m_linesColorButton->iconSize().width(),
                         d->m_linesColorButton->iconSize().height() );
    linesPixmap.fill( lineStyle.color() );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );

    QPixmap polyPixmap( d->m_polyColorButton->iconSize().width(),
                        d->m_polyColorButton->iconSize().height() );
    polyPixmap.fill( polyStyle.color() );
    d->m_polyColorButton->setIcon( QIcon( polyPixmap ) );

    // Setup the color dialogs.
    d->m_linesDialog = new QColorDialog( this );
    d->m_linesDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_linesDialog->setCurrentColor( lineStyle.color() );
    connect( d->m_linesColorButton, SIGNAL(clicked()), d->m_linesDialog, SLOT(exec()) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateLinesDialog(const QColor&)) );

    d->m_polyDialog = new QColorDialog( this );
    d->m_polyDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_polyDialog->setCurrentColor( polyStyle.color() );
    connect( d->m_polyColorButton, SIGNAL(clicked()), d->m_polyDialog, SLOT(exec()) );
    connect( d->m_polyDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updatePolyDialog(const QColor&)) );


    // Promote "Apply" button to default button and connect it to updatePolygon() slot.
    QPushButton *applyButton = d->buttonBox->button( QDialogButtonBox::Apply );
    d->buttonBox->button( QDialogButtonBox::Apply )->setDefault( true );
    connect( applyButton, SIGNAL(clicked()), this, SLOT(updatePolygon()) );

    // Make sure pressing "OK" will also update the polygon.
    connect( d->buttonBox, SIGNAL(accepted()), this, SLOT(updatePolygon()) );

    // Ensure that the dialog gets deleted when closing it (either when clicking OK or
    // Close).
    connect( this, SIGNAL(finished(int)), SLOT(deleteLater()) );
}

EditPolygonDialog::~EditPolygonDialog()
{
    delete d;
}

void EditPolygonDialog::updatePolygon()
{
    GeoDataStyle *style = new GeoDataStyle( *d->m_placemark->style() );

    d->m_placemark->setName( d->m_name->text() );
    d->m_placemark->setDescription( d->m_description->toPlainText() );

    style->lineStyle().setWidth( d->m_linesWidth->value() );
    // 0 corresponds to "Filled" and 1 corresponds to "Not Filled".
    style->polyStyle().setFill( !d->m_filledColor->currentIndex() );


    // Adjust the lines/polygon colors.
    // QColorDialog::currentColor() also works even if the color dialog
    // has not been exec'ed, while QColorDialog::selectedColor() does not.
    style->lineStyle().setColor( d->m_linesDialog->currentColor() );
    style->polyStyle().setColor( d->m_polyDialog->currentColor() );


    d->m_placemark->setStyle( style );
    emit polygonUpdated( d->m_placemark );
}

void EditPolygonDialog::updateLinesDialog( const QColor &color )
{
    QPixmap linesPixmap( d->m_linesColorButton->iconSize().width(),
                         d->m_linesColorButton->iconSize().height() );
    linesPixmap.fill( color );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );
}

void EditPolygonDialog::updatePolyDialog( const QColor &color )
{
    QPixmap polyPixmap( d->m_polyColorButton->iconSize().width(),
                        d->m_polyColorButton->iconSize().height() );
    polyPixmap.fill( color );
    d->m_polyColorButton->setIcon( QIcon( polyPixmap ) );
}

}

#include "EditPolygonDialog.moc"
