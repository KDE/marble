//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include "MarbleLegendBrowser.h"

#include <QtCore/QTime>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QPainter>
#include <QtGui/QTextFrame>
#include <QtGui/QScrollBar>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionButton>
#include <QtCore/QRegExp>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSettings.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"

#include "MarbleDirs.h"

namespace Marble
{

class MarbleLegendBrowserPrivate
{
 public:
    MarbleModel        *m_marbleModel;
    QMap<QString, bool>     m_checkBoxMap;
    QMap<QString, QPixmap>  m_symbolMap;
    bool                 m_isLegendLoaded;
};


// ================================================================


MarbleLegendBrowser::MarbleLegendBrowser( QWidget *parent )
    : QTextBrowser( parent ),
      d( new MarbleLegendBrowserPrivate )
{
    d->m_isLegendLoaded = false;
    d->m_marbleModel = 0;
    // Disable changing layout due to the ScrollBarPolicy:
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    setTextInteractionFlags( Qt::LinksAccessibleByMouse
                             | Qt::LinksAccessibleByKeyboard );

    setOpenLinks( false );

    connect ( this, SIGNAL( anchorClicked( QUrl ) ),
              this, SLOT( toggleCheckBoxStatus( QUrl ) ) );

}

MarbleLegendBrowser::~MarbleLegendBrowser()
{
    delete d;
}

void MarbleLegendBrowser::setMarbleModel( MarbleModel *marbleModel )
{
    // We need this to be able to get to the MapTheme.
    d->m_marbleModel = marbleModel;

    if ( d->m_marbleModel ) {
        connect ( d->m_marbleModel, SIGNAL( themeChanged( QString ) ),
                  this, SLOT( initTheme() ) );
    }
}

void MarbleLegendBrowser::initTheme()
{
    mDebug() << "initTheme";

    // Check for a theme specific legend.html first
    if ( d->m_marbleModel != 0 && d->m_marbleModel->mapTheme() != 0 )
    {
        const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        d->m_checkBoxMap.clear();

        foreach ( const GeoSceneProperty *property, currentMapTheme->settings()->allProperties() ) {
            if ( property->available() ) {
                d->m_checkBoxMap[ property->name() ] = property->value();
            }
        }

        disconnect ( currentMapTheme, SIGNAL( valueChanged( QString, bool ) ), 0, 0 );
        connect ( currentMapTheme, SIGNAL( valueChanged( QString, bool ) ),
                  this, SLOT( setCheckedProperty( QString, bool ) ) );
    }

    if ( isVisible() ) {
        loadLegend();
    }
    else {
        d->m_isLegendLoaded = false;
    }
}

void MarbleLegendBrowser::loadLegend()
{
    mDebug() << "loadLegend";
    QTime t;
    t.start();

    // Read the html string.
    QString legendPath;

    // Check for a theme specific legend.html first
    if ( d->m_marbleModel != 0 && d->m_marbleModel->mapTheme() != 0 )
    {
        GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        legendPath = MarbleDirs::path( "maps/" + 
        currentMapTheme->head()->target() + '/' + 
        currentMapTheme->head()->theme() + "/legend.html" ); 
    }
    if ( legendPath.isEmpty() ) {
	legendPath = MarbleDirs::path( "legend.html" ); 
    }

    QString finalHtml = readHtml( QUrl::fromLocalFile( legendPath ) );
    finalHtml.replace( QString( "./" ), legendPath.section( '/', 0, -2 ) + '/' );

    // Generate some parts of the html from the MapTheme <Legend> tag. 
    const QString sectionsHtml = generateSectionsHtml();

    // And then create the final html from these two parts.
    finalHtml.replace( QString( "<!-- ##customLegendEntries:all## -->" ), sectionsHtml );

    translateHtml( finalHtml );

    // Set the html string in the QTextBrowser.
    setHtml( finalHtml );

    QTextFrameFormat  format = document()->rootFrame()->frameFormat();
    format.setMargin(6);
    document()->rootFrame()->setFrameFormat( format );
    viewport()->update();

    d->m_isLegendLoaded = true;
    qDebug("loadLegend: Time elapsed: %d ms", t.elapsed());

    if ( d->m_marbleModel ) {
        d->m_marbleModel->setLegend( document() );
    }
}

bool MarbleLegendBrowser::event( QEvent * event )
{
    // "Delayed initialization": legend gets created only 
    if ( event->type() == QEvent::Show ) {
        if ( !d->m_isLegendLoaded ) {
            loadLegend();
            return true;
        }
    }
    return QTextBrowser::event( event );
}

QString MarbleLegendBrowser::readHtml( const QUrl & name )
{
    QString html;

    QFile data( name.toLocalFile() );
    if ( data.open( QFile::ReadOnly ) ) {
        QTextStream in( &data );
        html = in.readAll();
        data.close();
    }

    // Tell QTextBrowser where to search for further document resources
    QStringList paths = searchPaths();
    paths.append( QFileInfo(data).absolutePath() );
    setSearchPaths( paths );
    return html;
}

void MarbleLegendBrowser::translateHtml( QString & html )
{
    // must match string extraction in Messages.sh
    QString s = html.remove( 0, html.indexOf( "<body>" ) );
    QRegExp rx( "</?\\w+((\\s+\\w+(\\s*=\\s*(?:\".*\"|'.*'|[^'\">\\s]+))?)+\\s*|\\s*)/?>" );
    rx.setMinimal( true );
    s.replace( rx, "\n" );
    s.replace( QRegExp( "\\s*\n\\s*" ), "\n" );
    QStringList words = s.split( '\n', QString::SkipEmptyParts );

    QStringList::const_iterator i = words.constBegin();
    QStringList::const_iterator const end = words.constEnd();
    for (; i != end; ++i )
        html.replace( *i, tr( (*i).toUtf8() ) );
}

QString MarbleLegendBrowser::generateSectionsHtml()
{
    // Generate HTML to include into legend.html here.

    QString customLegendString;

    if ( d->m_marbleModel == 0 || d->m_marbleModel->mapTheme() == 0 )
        return QString();

    GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

    d->m_symbolMap.clear();

    int sectionNum = 0;
    foreach ( const GeoSceneSection *section, currentMapTheme->legend()->sections() ) {
        QString checkBoxString; 

        if ( section->checkable() ) {
            checkBoxString = "<a href=\"checkbox:" + section->connectTo() + "\"><span style=\"text-decoration: none\"><img src=\"checkbox:" + section->name() + "\">&nbsp;</span></a> ";
        }

        customLegendString += "<h4>" + checkBoxString + section->heading() + "</h4>";
        customLegendString += "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\">";

        int spacing = section->spacing();

        int itemNum = 0;
        foreach ( const GeoSceneItem *item, section->items() ) {

            QPixmap itemPixmap;
            QString pixmapRelativePath = item->icon()->pixmap();

            if ( !pixmapRelativePath.isEmpty() ) {
                QString pixmapPath = MarbleDirs::path( pixmapRelativePath );
                itemPixmap = QPixmap( pixmapPath );
            }
            else
                itemPixmap = QPixmap( 24, 12 );

            QPixmap itemIcon = itemPixmap.copy();

            QColor itemColor = item->icon()->color();
            itemIcon.fill( itemColor );

            QPainter painter( &itemIcon );

            if ( !pixmapRelativePath.isEmpty() ) {
                painter.drawPixmap( 0, 0, itemPixmap );
            }


            QString itemIdString = QString("item%1-%2").arg(sectionNum).arg(itemNum);
            d->m_symbolMap[itemIdString] = itemIcon;

            customLegendString += "    <tr>";
            customLegendString += QString( "        <td align=\"left\" valign=\"top\" width=\"%1\">" ).arg( itemIcon.width() + spacing ); 
            customLegendString += "             <img src=\"pixmap:" + itemIdString + "\">";
            customLegendString += "        </td>"; 
            customLegendString += "        <td align=\"left\" valign=\"top}\">"; 
            customLegendString += "             " + item->text();
            customLegendString += "        </td>"; 
            customLegendString += "    </tr>";

            itemNum++;
        }
        customLegendString += "</table>";

        sectionNum++;
    }

    return customLegendString;
}


//
// This function is reimplemented from QTextBrowser to handle the
// checkboxes.  For all other resources, it reuses
// QTextBrowser::loadResource.
//    
QVariant MarbleLegendBrowser::loadResource ( int type, const QUrl & name )
{
    QString  newName;

    if ( type == QTextDocument::ImageResource
         && name.toString().startsWith("checkbox:", Qt::CaseInsensitive) )
    {
        QStyleOptionButton option;
        option.initFrom(this);
        int width = style()->pixelMetric(QStyle::PM_IndicatorWidth, &option, this );
        int height = style()->pixelMetric(QStyle::PM_IndicatorHeight, &option, this );
        option.rect = QRect( 0, 0, width, height );
        
        QString checkBoxName = name.toString().section(':', 1, -1);
        if ( !d->m_checkBoxMap.contains( checkBoxName ) ) {
            option.state = QStyle::State_None;
        }
        else if ( d->m_checkBoxMap.value( checkBoxName ) )
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;
            
        QPixmap pixmap( width, height );
        pixmap.fill( Qt::transparent );

        QPainter painter;
        painter.begin(&pixmap);
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, &painter, this);
        painter.end();

        return pixmap;
    }

    if ( type == QTextDocument::ImageResource
         && name.toString().startsWith("pixmap:", Qt::CaseInsensitive) )
    {
        QString pixmapName = name.toString().section(':', 1, -1);
        if ( d->m_symbolMap.contains( pixmapName ) ) {
            return d->m_symbolMap.value( pixmapName );
        }
    }

    return QTextBrowser::loadResource( type, name );
}

void MarbleLegendBrowser::toggleCheckBoxStatus( const QUrl &link )
{
    // If we got an HTTP Url, open a browser window 
    if ( link.scheme() == "http" || link.scheme() == "https" ) {
        QDesktopServices::openUrl( link );
        return;
    }

    if ( link.scheme() == "checkbox" ) {
        QString checkBoxName = link.toString().section(':', 1, -1);

        if ( d->m_checkBoxMap.contains( checkBoxName ) ) {
            d->m_checkBoxMap[ checkBoxName ] = !d->m_checkBoxMap.value( checkBoxName );
            emit toggledShowProperty( checkBoxName, d->m_checkBoxMap.value( checkBoxName ) );
        }
    }

    setUpdatesEnabled( false );
    {
        int scrollPosition = verticalScrollBar()->sliderPosition();

        loadLegend();

        verticalScrollBar()->setSliderPosition( scrollPosition );
    }
    setUpdatesEnabled( true );

    update();
}

void MarbleLegendBrowser::setCheckedProperty( const QString& name, bool checked )
{
    // If there is no change then leave immediately
    if ( d->m_checkBoxMap[ name ] == checked )
        return;
    
    d->m_checkBoxMap[ name ] = checked;

    setUpdatesEnabled( false );
    {
        int scrollPosition = verticalScrollBar()->sliderPosition();
        loadLegend();

        verticalScrollBar()->setSliderPosition( scrollPosition );
    }
    setUpdatesEnabled( true );

    update();
}

}

#include "MarbleLegendBrowser.moc"
