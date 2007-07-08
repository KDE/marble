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


#include "LegendBrowser.h"

#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtGui/QTextFrame>
#include <QtGui/QScrollBar>

#include "katlasdirs.h"


class LegendBrowserPrivate
{
 public:
    QMap<QString, bool>  m_checkBoxMap;
};


// ================================================================


LegendBrowser::LegendBrowser( QWidget *parent )
    : QTextBrowser( parent ),
      d( new LegendBrowserPrivate )
{
    setSource( KAtlasDirs::path( "legend.html" ) );
    QTextFrameFormat  format = document()->rootFrame()->frameFormat();
    format.setMargin(6);
    document()->rootFrame()->setFrameFormat( format );

    setTextInteractionFlags( Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard );

    connect ( this, SIGNAL( anchorClicked( QUrl ) ), this, SLOT( toggleCheckBoxStatus( QUrl ) ) );
}

QVariant LegendBrowser::loadResource ( int type, const QUrl & name )
{

    if ( type == QTextDocument::ImageResource && name.toString().startsWith("checkbox:", Qt::CaseInsensitive) )
    {
        QString checkBoxName = name.toString().section(":", 1, -1);
        if ( !d->m_checkBoxMap.contains( checkBoxName ) )
        {
            return QTextBrowser::loadResource( type, QUrl("./bitmaps/checkbox_disabled.png") );
//            d->m_checkBoxMap[ checkBoxName ] = false;
        }
        if ( d->m_checkBoxMap.value( checkBoxName ) == true )
        {
            return QTextBrowser::loadResource( type, QUrl("./bitmaps/checkbox_checked.png") );
        }
        else
        {
            return QTextBrowser::loadResource( type, QUrl("./bitmaps/checkbox_empty.png") );
        }
    }
    else
    {
        return QTextBrowser::loadResource( type, name );
    }
}

void LegendBrowser::toggleCheckBoxStatus( const QUrl &link )
{
    if ( link.toString().startsWith( "checkbox:", Qt::CaseInsensitive ) )
    {
        QString checkBoxName = link.toString().section(":", 1, -1);
        if ( d->m_checkBoxMap.contains( checkBoxName ) )
        {
            d->m_checkBoxMap[ checkBoxName ] = ( d->m_checkBoxMap.value( checkBoxName ) == true ) ? false : true;
            sendSignals( checkBoxName, d->m_checkBoxMap.value( checkBoxName ) );
        }
    }
    setUpdatesEnabled( false );
    int scrollPosition = verticalScrollBar()->sliderPosition();
    setSource( KAtlasDirs::path( "legend.html" ) );
    QTextFrameFormat  format = document()->rootFrame()->frameFormat();
    format.setMargin(6);
    document()->rootFrame()->setFrameFormat( format );
    verticalScrollBar()->setSliderPosition( scrollPosition );
    setUpdatesEnabled( true );
    repaint();
}

void LegendBrowser::sendSignals( const QString &name, bool checked )
{
    if ( name == "locations"){
        emit toggledLocations( checked );
    }
    if ( name == "cities"){
        emit toggledCities( checked );
    }
    if ( name == "terrain"){
        emit toggledTerrain( checked );
    }
    if ( name == "borders"){
        emit toggledBorders( checked );
    }
    if ( name == "waterbodies"){
        emit toggledWaterBodies( checked );
    }
    if ( name == "grid"){
        emit toggledGrid( checked );
    }
    if ( name == "ice"){
        emit toggledIceLayer( checked );
    }
    if ( name == "relief"){
        emit toggledRelief( checked );
    }
    if ( name == "windrose"){
        emit toggledWindRose( checked );
    }
    if ( name == "scalebar"){
        emit toggledScaleBar( checked );
    }
}


// ----------------------------------------------------------------
//  Lots of slots


void LegendBrowser::setCheckedLocations( bool checked )
{
    d->m_checkBoxMap[ "locations" ] = checked;
}

void LegendBrowser::setCheckedCities( bool checked )
{
    d->m_checkBoxMap[ "cities" ] = checked;
}

void LegendBrowser::setCheckedTerrain( bool checked )
{
    d->m_checkBoxMap[ "terrain" ] = checked;
}

void LegendBrowser::setCheckedBorders( bool checked )
{
    d->m_checkBoxMap[ "borders" ] = checked;
}

void LegendBrowser::setCheckedWaterBodies( bool checked )
{
    d->m_checkBoxMap[ "waterbodies" ] = checked;
}

void LegendBrowser::setCheckedIceLayer( bool checked )
{
    d->m_checkBoxMap[ "ice" ] = checked;
}

void LegendBrowser::setCheckedGrid( bool checked )
{
    d->m_checkBoxMap[ "grid" ] = checked;
}

void LegendBrowser::setCheckedRelief( bool checked )
{
    d->m_checkBoxMap[ "relief" ] = checked;
}

void LegendBrowser::setCheckedWindRose( bool checked )
{
    d->m_checkBoxMap[ "windrose" ] = checked;
}

void LegendBrowser::setCheckedScaleBar( bool checked )
{
    d->m_checkBoxMap[ "scalebar" ] = checked;
}



#include "LegendBrowser.moc"
