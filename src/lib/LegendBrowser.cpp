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

LegendBrowser::LegendBrowser( QWidget *parent )
    : QTextBrowser( parent )
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
        if ( !m_checkBoxMap.contains( checkBoxName ) )
        {
            return QTextBrowser::loadResource( type, QUrl("./bitmaps/checkbox_disabled.png") );
//            m_checkBoxMap[ checkBoxName ] = false;
        }
        if ( m_checkBoxMap.value( checkBoxName ) == true )
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

void LegendBrowser::toggleCheckBoxStatus( QUrl link )
{
    if ( link.toString().startsWith( "checkbox:", Qt::CaseInsensitive ) )
    {
        QString checkBoxName = link.toString().section(":", 1, -1);
        if ( m_checkBoxMap.contains( checkBoxName ) )
        {
            m_checkBoxMap[ checkBoxName ] = ( m_checkBoxMap.value( checkBoxName ) == true ) ? false : true;
            sendSignals( checkBoxName, m_checkBoxMap.value( checkBoxName ) );
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

void LegendBrowser::sendSignals( QString name, bool checked )
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

#include "LegendBrowser.moc"
