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
#include <QtGui/QTextFrame>
#include <QtGui/QScrollBar>
#include <QRegExp>

#include "MarbleDirs.h"


class MarbleLegendBrowserPrivate
{
 public:
    MarbleWidget        *m_marbleWidget;
    QMap<QString, bool>  m_checkBoxMap;
    QString              m_html;
    QString              m_loadedSectionsHtml;
};


// ================================================================


MarbleLegendBrowser::MarbleLegendBrowser( QWidget *parent )
    : QTextBrowser( parent ),
      d( new MarbleLegendBrowserPrivate )
{
    // Disable changing layout due to the ScrollBarPolicy:
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    // Read the html string.
    d->m_html = readHtml( QUrl::fromLocalFile( MarbleDirs::path( "legend.html" ) ) );

    // Generate some parts of the html from the MapTheme <Legend> tag. 
    d->m_loadedSectionsHtml = genSectionsHtml();

    // And then create the final html from these two parts.
    QString  finalHtml = d->m_html;
    finalHtml.replace( QString( "##genhtml##" ), d->m_loadedSectionsHtml );

    // Set the html string in the QTextBrowser.
    setHtml( finalHtml );

    QTextFrameFormat  format = document()->rootFrame()->frameFormat();
    format.setMargin(6);
    document()->rootFrame()->setFrameFormat( format );
    viewport()->update();

    setTextInteractionFlags( Qt::LinksAccessibleByMouse
                             | Qt::LinksAccessibleByKeyboard );

    connect ( this, SIGNAL( anchorClicked( QUrl ) ),
              this, SLOT( toggleCheckBoxStatus( QUrl ) ) );
}

MarbleLegendBrowser::~MarbleLegendBrowser()
{
    delete d;
}

 
void MarbleLegendBrowser::setMarbleWidget( MarbleWidget *marbleWidget )
{
    // We need this to be able to get to the MapTheme.
    d->m_marbleWidget = marbleWidget;
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

    QStringList paths = searchPaths();
    paths.append( QFileInfo(data).absolutePath() );
    setSearchPaths( paths );

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

    return html;
}

QString MarbleLegendBrowser::genSectionsHtml()
{
    // Generate HTML to include into legend.html here.
    //mapTheme = d->m_marbleWidget->model()->mapThemeObject();

    // FIXME: Implement this function

    return QString();
}


//
// This function is reimplemented from QTextBrowser to handle the
// checkboxes.  For all other resources, it reuses
// QTextBrowser::loadResource.
//    
QVariant MarbleLegendBrowser::loadResource ( int type, const QUrl & name )
{
    QString  newName;

    if ( type != QTextDocument::ImageResource
         || !name.toString().startsWith("checkbox:", Qt::CaseInsensitive) )
        return QTextBrowser::loadResource( type, name );        

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
        setHtml( d->m_html );

        QTextFrameFormat  format = document()->rootFrame()->frameFormat();
        format.setMargin(6);
        document()->rootFrame()->setFrameFormat( format );

        verticalScrollBar()->setSliderPosition( scrollPosition );
    }
    setUpdatesEnabled( true );

    repaint();
}

void MarbleLegendBrowser::sendSignals( const QString &name, bool checked )
{
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
