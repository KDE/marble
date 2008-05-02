//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "MarbleLegendBrowser.h"

#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QPainter>
#include <QtGui/QTextFrame>
#include <QtGui/QScrollBar>
#include <QtCore/QRegExp>

#include "GeoSceneDocument.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"

#include "MarbleDirs.h"


class MarbleLegendBrowserPrivate
{
 public:
    MarbleWidget        *m_marbleWidget;
    QMap<QString, bool>     m_checkBoxMap;
    QMap<QString, QPixmap>  m_symbolMap;
    QString              m_html;
    QString              m_loadedSectionsHtml;
};


// ================================================================


MarbleLegendBrowser::MarbleLegendBrowser( QWidget *parent )
    : QTextBrowser( parent ),
      d( new MarbleLegendBrowserPrivate )
{
    d->m_marbleWidget = 0;
    // Disable changing layout due to the ScrollBarPolicy:
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    setTextInteractionFlags( Qt::LinksAccessibleByMouse
                             | Qt::LinksAccessibleByKeyboard );

    connect ( this, SIGNAL( anchorClicked( QUrl ) ),
              this, SLOT( toggleCheckBoxStatus( QUrl ) ) );

   loadLegend();
}

MarbleLegendBrowser::~MarbleLegendBrowser()
{
    delete d;
}

void MarbleLegendBrowser::loadLegend()
{
    qDebug() << "loadLegend";

    // Read the html string.

    // Check for a theme specific legend.html first
    if ( d->m_marbleWidget != 0
	 && d->m_marbleWidget->model() != 0
	 && d->m_marbleWidget->model()->mapTheme() != 0 )
    {
        GeoSceneDocument *currentMapTheme = d->m_marbleWidget->model()->mapTheme();

        disconnect ( currentMapTheme, SIGNAL( valueChanged( QString, bool ) ), 0, 0 );
        connect ( currentMapTheme, SIGNAL( valueChanged( QString, bool ) ),
                  this, SLOT( test( QString, bool ) ) );

        QString customLegendPath = MarbleDirs::path( "maps/earth/" + currentMapTheme->head()->theme() + "/legend.html" ); 
        if ( !customLegendPath.isEmpty() )
            d->m_html = readHtml( QUrl::fromLocalFile( customLegendPath  ) );
        else
            d->m_html = "";
    }

    if ( d->m_html.isEmpty() ) {
        d->m_html = readHtml( QUrl::fromLocalFile( MarbleDirs::path( "legend.html" ) ) );
    }

    // Generate some parts of the html from the MapTheme <Legend> tag. 
    d->m_loadedSectionsHtml = generateSectionsHtml();

    // And then create the final html from these two parts.
    QString  finalHtml = d->m_html;
    finalHtml.replace( QString( "<!-- ##customLegendEntries:all## -->" ), d->m_loadedSectionsHtml );

    translateHtml( finalHtml );

    // Set the html string in the QTextBrowser.
    setHtml( finalHtml );

    QTextFrameFormat  format = document()->rootFrame()->frameFormat();
    format.setMargin(6);
    document()->rootFrame()->setFrameFormat( format );
    viewport()->update();
}
 
void MarbleLegendBrowser::setMarbleWidget( MarbleWidget *marbleWidget )
{
    // We need this to be able to get to the MapTheme.
    d->m_marbleWidget = marbleWidget;

    loadLegend();

    if ( d->m_marbleWidget ) {
        connect ( d->m_marbleWidget, SIGNAL( themeChanged( QString ) ),
                  this, SLOT( loadLegend() ) );

        // FIXME: This doesn't do anything yet and needs improvement
    }
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
    QStringList words = s.split( "\n", QString::SkipEmptyParts );

    for (QStringList::const_iterator i = words.constBegin(); 
         i != words.constEnd(); ++i)
        html.replace( *i, tr( (*i).toUtf8() ) );
}

QString MarbleLegendBrowser::generateSectionsHtml()
{
    // Generate HTML to include into legend.html here.

    QString customLegendString;

    if ( d->m_marbleWidget == 0 || d->m_marbleWidget->model() == 0 || d->m_marbleWidget->model()->mapTheme() == 0 )
        return QString();

    GeoSceneDocument *currentMapTheme = d->m_marbleWidget->model()->mapTheme();

    QVector<GeoSceneSection*> sections = currentMapTheme->legend()->sections();

    d->m_symbolMap.clear();

    for (int section = 0; section < sections.size(); ++section) {
        QString checkBoxString; 

        if ( sections.at(section)->checkable() == true ) {
            checkBoxString = "<a href=\"checkbox:" + sections.at(section)->connectTo() + "\"><span style=\"text-decoration: none\"><img src=\"checkbox:" + sections.at(section)->name() + "\">&nbsp;</span></a> ";
        }

        customLegendString += "<h4>" + checkBoxString + sections.at(section)->heading() + "</h4>";
        customLegendString += "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\">";

        QVector<GeoSceneItem*> items = sections.at(section)->items();

        int spacing = sections.at(section)->spacing();

        for (int item = 0; item < items.size(); ++item) {

            QPixmap itemPixmap;
            QString pixmapRelativePath = items.at(item)->icon()->pixmap();

            if ( !pixmapRelativePath.isEmpty() ) {
                QString pixmapPath = MarbleDirs::path( pixmapRelativePath );
                itemPixmap = QPixmap( pixmapPath );
            }
            else
                itemPixmap = QPixmap( 24, 12 );

            QPixmap itemIcon = itemPixmap;
            QColor itemColor = QColor( items.at(item)->icon()->color() );

            if ( itemColor != Qt::transparent )
            {
                QPainter painter( &itemIcon );
                painter.fillRect( QRect( 0, 0, itemIcon.width(), itemIcon.height() ), itemColor);
                // Paint the pixmap on top of the colored background
                if ( !pixmapRelativePath.isEmpty() ) {
                    painter.drawPixmap( 0, 0, itemPixmap );
                }
            }


            QString itemIdString = QString("item%1-%2").arg(section).arg(item);
            d->m_symbolMap[itemIdString] = itemIcon;

            customLegendString += "    <tr>";
            customLegendString += QString( "        <td align=\"left\" valign=\"top\" width=\"%1\">" ).arg( itemIcon.width() + spacing ); 
            customLegendString += "             <img src=\"pixmap:" + itemIdString + "\">";
            customLegendString += "        </td>"; 
            customLegendString += "        <td align=\"left\" valign=\"top}\">"; 
            customLegendString += "             " + items.at(item)->text();
            customLegendString += "        </td>"; 
            customLegendString += "    </tr>";
        }
        customLegendString += "</table>";
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
        QString checkBoxName = name.toString().section(":", 1, -1);
        if ( !d->m_checkBoxMap.contains( checkBoxName ) ) {
            newName = "./bitmaps/checkbox_disabled.png";
        }
        else if ( d->m_checkBoxMap.value( checkBoxName ) )
            newName = "./bitmaps/checkbox_checked.png";
        else
            newName = "./bitmaps/checkbox_empty.png";

        return QTextBrowser::loadResource( type, QUrl( newName ) );
    }

    if ( type == QTextDocument::ImageResource
         && name.toString().startsWith("pixmap:", Qt::CaseInsensitive) )
    {
        QString pixmapName = name.toString().section(":", 1, -1);
        if ( d->m_symbolMap.contains( pixmapName ) ) {
            return d->m_symbolMap.value( pixmapName );
        }
    }

    return QTextBrowser::loadResource( type, name );
}

void MarbleLegendBrowser::toggleCheckBoxStatus( const QUrl &link )
{
    if ( link.toString().startsWith( "checkbox:", Qt::CaseInsensitive ) ) {
        QString checkBoxName = link.toString().section(":", 1, -1);

        if ( d->m_checkBoxMap.contains( checkBoxName ) ) {
            d->m_checkBoxMap[ checkBoxName ] = !d->m_checkBoxMap.value( checkBoxName );
            sendSignals( checkBoxName, d->m_checkBoxMap.value( checkBoxName ) );
        }
    }

    setUpdatesEnabled( false );
    {
        int scrollPosition = verticalScrollBar()->sliderPosition();

        loadLegend();

        verticalScrollBar()->setSliderPosition( scrollPosition );
    }
    setUpdatesEnabled( true );

    repaint();
}

void MarbleLegendBrowser::sendSignals( const QString &name, bool checked )
{
    emit toggledShowProperty( name, checked );

    if ( name == "locations") {
        emit toggledLocations( checked );
    }
    if ( name == "cities") {
        emit toggledCities( checked );
    }
    if ( name == "terrain") {
        emit toggledTerrain( checked );
    }

    if ( name == "borders") {
        emit toggledBorders( checked );
    }
    if ( name == "waterbodies") {
        emit toggledWaterBodies( checked );
    }
    if ( name == "otherplaces") {
        emit toggledOtherPlaces( checked );
    }
    if ( name == "grid") {
        emit toggledGrid( checked );
    }
    if ( name == "ice") {
        emit toggledIceLayer( checked );
    }
    if ( name == "relief") {
        emit toggledRelief( checked );
    }
    if ( name == "compass") {
        emit toggledCompass( checked );
    }
    if ( name == "scalebar") {
        emit toggledScaleBar( checked );
    }
}


// ----------------------------------------------------------------
//  Lots of slots


void MarbleLegendBrowser::setCheckedLocations( bool checked )
{
    d->m_checkBoxMap[ "locations" ] = checked;
}

void MarbleLegendBrowser::setCheckedCities( bool checked )
{
    d->m_checkBoxMap[ "cities" ] = checked;
}

void MarbleLegendBrowser::setCheckedTerrain( bool checked )
{
    d->m_checkBoxMap[ "terrain" ] = checked;
}

void MarbleLegendBrowser::setCheckedBorders( bool checked )
{
    d->m_checkBoxMap[ "borders" ] = checked;
}

void MarbleLegendBrowser::setCheckedWaterBodies( bool checked )
{
    d->m_checkBoxMap[ "waterbodies" ] = checked;
}

void MarbleLegendBrowser::setCheckedOtherPlaces( bool checked )
{
    d->m_checkBoxMap[ "otherplaces" ] = checked;
}

void MarbleLegendBrowser::setCheckedIceLayer( bool checked )
{
    d->m_checkBoxMap[ "ice" ] = checked;
}

void MarbleLegendBrowser::setCheckedGrid( bool checked )
{
    d->m_checkBoxMap[ "grid" ] = checked;
}

void MarbleLegendBrowser::setCheckedRelief( bool checked )
{
    d->m_checkBoxMap[ "relief" ] = checked;
}

void MarbleLegendBrowser::setCheckedCompass( bool checked )
{
    d->m_checkBoxMap[ "compass" ] = checked;
}

void MarbleLegendBrowser::setCheckedScaleBar( bool checked )
{
    d->m_checkBoxMap[ "scalebar" ] = checked;
}



#include "MarbleLegendBrowser.moc"
