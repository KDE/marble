//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "License.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleAboutDialog.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "ViewportParams.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneLicense.h"

#include <QCommonStyle>
#include <QContextMenuEvent>
#include <QPainter>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>

namespace Marble
{

class OutlinedStyle : public QCommonStyle {
public:
    void drawItemText( QPainter *painter, const QRect &rect, int alignment, const QPalette &palette,
                       bool enabled, const QString& text, QPalette::ColorRole textRole ) const override {
        Q_UNUSED( alignment );
        Q_UNUSED( enabled );

        if ( text.isEmpty() ) {
            return;
        }

        QPen savedPen;
        if ( textRole != QPalette::NoRole ) {
            savedPen = painter->pen();
            painter->setPen( QPen( palette.brush( textRole ), savedPen.widthF() ) );
        }

        QPainterPath path;
        QFontMetricsF metrics( painter->font() );
        QPointF point( rect.x() + 7.0, rect.y() + metrics.ascent() );
        path.addText( point, painter->font(), text );
        QPen pen( Qt::white );
        pen.setWidth( 3 );
        painter->setPen( pen );
        painter->setBrush( QBrush( Qt::black ) );
        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->drawPath( path );

        painter->setPen( Qt::NoPen );
        painter->drawPath( path );

        if ( textRole != QPalette::NoRole ) {
            painter->setPen( savedPen );
        }
    }
};

License::License( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( -10.0, -5.0 ), QSizeF( 150.0, 20.0 ) ),
      m_widgetItem( 0 ),
      m_label( 0 ),
      m_showFullLicense( false ),
      m_contextMenu( 0 )
{
    setEnabled( true );
    setVisible( true );
    setBackground( QBrush( QColor( Qt::transparent ) ) );
    setFrame( NoFrame );
}

License::~License()
{
}

QStringList License::backendTypes() const
{
    return QStringList(QStringLiteral("License"));
}

QString License::name() const
{
    return tr( "License" );
}

QString License::guiString() const
{
    return tr( "&License" );
}

QString License::nameId() const
{
    return QStringLiteral("license");
}

QString License::version() const
{
    return QStringLiteral("1.0");
}

QString License::description() const
{
    return tr( "This is a float item that provides copyright information." );
}

QString License::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> License::pluginAuthors() const
{
    return QVector<PluginAuthor>()
           << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
           << PluginAuthor(QStringLiteral("Illya Kovalevskyy"), QStringLiteral("illya.kovalevskyy@gmail.com"));
}

QIcon License::icon () const
{
    return QIcon(QStringLiteral(":/icons/license.png"));
}

void License::initialize ()
{
    delete m_widgetItem;
    m_widgetItem = new WidgetGraphicsItem( this );
    m_label = new QLabel;
    auto style = new OutlinedStyle;
    style->setParent(this);
    m_label->setStyle( style );
    m_label->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    m_widgetItem->setWidget( m_label );

    MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
    layout->addItem( m_widgetItem, 0, 0 );
    setLayout( layout );
    setPadding( 0 );

    updateLicenseText();
    connect( marbleModel(), SIGNAL(themeChanged(QString)), this, SLOT(updateLicenseText()) );
}

void License::updateLicenseText()
{
    const GeoSceneDocument *const mapTheme = marbleModel()->mapTheme();
    if ( !mapTheme )
        return;

    const GeoSceneHead *const head = mapTheme->head();
    if ( !head )
        return;

    const GeoSceneLicense *license = marbleModel()->mapTheme()->head()->license();
    m_label->setText( m_showFullLicense ? license->license() : license->shortLicense() );
    m_label->setToolTip( license->license() );
    if( license->attribution() == GeoSceneLicense::Always ) {
        setUserCheckable( false );
    } else if( license->attribution() == GeoSceneLicense::Never ) {
        setVisible( false );
        setUserCheckable( false );
    } else if( license->attribution() == GeoSceneLicense::OptIn ) {
        setUserCheckable( true );
        setVisible( false );
    } else {
        setUserCheckable( true );
        setVisible( true );
    }
    QSizeF const magic( 6,0 );
    m_widgetItem->setSize( m_label->sizeHint()+magic );
    setSize( m_label->sizeHint()+magic );
    update();
    emit repaintNeeded();
}

void License::toggleLicenseSize()
{
    m_showFullLicense = !m_showFullLicense;
    updateLicenseText();
}

void License::showAboutDialog()
{
    QPointer<MarbleAboutDialog> aboutDialog = new MarbleAboutDialog;
    aboutDialog->setInitialTab( MarbleAboutDialog::Data );
    aboutDialog->exec();
    delete aboutDialog;
}

bool License::isInitialized () const
{
    return m_widgetItem;
}

bool License::eventFilter( QObject *object, QEvent *event )
{
    if ( !enabled() || !visible() )
        return false;

    MarbleWidget *widget = dynamic_cast<MarbleWidget*>( object );
    if ( !widget ) {
        return AbstractFloatItem::eventFilter( object,event );
    }

    if( event->type() == QEvent::MouseMove ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );
        QRectF floatItemRect = QRectF( positivePosition(), size() );
        if ( floatItemRect.contains( mouseEvent->pos() ) ) {
            widget->setCursor( QCursor( Qt::ArrowCursor ) );
            return true;
        }
    }

    return AbstractFloatItem::eventFilter( object, event );
}

void License::contextMenuEvent( QWidget *w, QContextMenuEvent *e )
{
    if ( !m_contextMenu ) {
        m_contextMenu = contextMenu();

        QAction *toggleAction = m_contextMenu->addAction( tr("&Full License"), this,
                                                SLOT(toggleLicenseSize()) );
        toggleAction->setCheckable( true );
        toggleAction->setChecked( m_showFullLicense );

        m_contextMenu->addAction( tr("&Show Details"), this, SLOT(showAboutDialog()) );
    }

    Q_ASSERT( m_contextMenu );
    m_contextMenu->exec( w->mapToGlobal( e->pos() ) );
}

}

#include "moc_License.cpp"
