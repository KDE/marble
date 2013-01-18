//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn      <tackat@kde.org>
// Copyright 2007      Inge Wallin       <ingwa@kde.org>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
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
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QTextDocument>

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
    : MarbleWebView( parent ),
      d( new MarbleLegendBrowserPrivate )
{
    d->m_isLegendLoaded = false;
    d->m_marbleModel = 0;

    QWebFrame *frame = page()->mainFrame();
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(injectCheckBoxChecker()));
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

    // Read the html string.
    QString legendPath;

    // Check for a theme specific legend.html first
    if ( d->m_marbleModel != 0 && d->m_marbleModel->mapTheme() != 0 )
    {
        const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        legendPath = MarbleDirs::path( "maps/" + 
        currentMapTheme->head()->target() + '/' + 
        currentMapTheme->head()->theme() + "/legend.html" ); 
    }
    if ( legendPath.isEmpty() ) {
        legendPath = MarbleDirs::path( "legend.html" );
    }

    QString finalHtml = readHtml( QUrl::fromLocalFile( legendPath ) );

    reverseSupportCheckboxes(finalHtml);

    // Generate some parts of the html from the MapTheme <Legend> tag. 
    const QString sectionsHtml = generateSectionsHtml();

    // And then create the final html from these two parts.
    finalHtml.replace( QString( "<!-- ##customLegendEntries:all## -->" ), sectionsHtml );

    translateHtml( finalHtml );

    QUrl baseUrl = QUrl::fromLocalFile( legendPath );

    // Set the html string in the QTextBrowser.
    setHtml(finalHtml, baseUrl);

    if ( d->m_marbleModel ) {
        QTextDocument *document = new QTextDocument(page()->mainFrame()->toHtml());
        d->m_marbleModel->setLegend( document );
    }
}

void MarbleLegendBrowser::injectCheckBoxChecker()
{
    QWebFrame *frame = page()->mainFrame();
    frame->addToJavaScriptWindowObject( "Marble", this );
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

    return MarbleWebView::event( event );
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

void MarbleLegendBrowser::reverseSupportCheckboxes(QString &html)
{
    const QString findEntry = "<a href=\"checkbox:";
    int ind = html.indexOf(findEntry, 0, Qt::CaseInsensitive);
    while (ind > 0) {
        QString id = "";
        int xInd = ind + findEntry.length();
        while (html[xInd].isLetterOrNumber() && xInd != html.length()) {
            id += html[xInd];
            xInd++;
        }
        int fin = html.indexOf("</a>", ind, Qt::CaseInsensitive)+4; // 4 = length "</a>";

        html.remove(ind, fin-ind);

        ind = html.indexOf(findEntry, 0, Qt::CaseInsensitive);
    }
}

QString MarbleLegendBrowser::generateSectionsHtml()
{
    // Generate HTML to include into legend.html here.

    QString customLegendString;

    if ( d->m_marbleModel == 0 || d->m_marbleModel->mapTheme() == 0 )
        return QString();

    const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

    d->m_symbolMap.clear();

    QString bitStyle = "<style>"
            "* {"
            "   font: \"sans-serif\"!important;"
            "}"
            ".spec {"
            "   display: block;"
            "}"
            ".spec h3 {"
            "   height: 30px;"
            "   margin: 0;"
            "   font-weight: 500;"
            "   font-size: 1em;"
            "}"
            "table {"
            "   margin-bottom: 20px!important;"
            "}"
            "table td.icon {"
            "   width: 24px;"
            "   overflow: hidden;"
            "   display: inline-block;"
            "   white-space: nowrap;"
            "   text-align: center;"
            "   vertical-align: middle;"
            "}"
            "table td.text {"
            "   width: 200px;"
            "   overflow: hidden;"
            "   display: inline-block;"
            "   white-space: nowrap;"
            "}"
            "</style>";

    customLegendString += bitStyle;

    foreach ( const GeoSceneSection *section, currentMapTheme->legend()->sections() ) {
        QString checkBoxString; 
        if (section->checkable()) {
            QString checked = "";
            if (d->m_checkBoxMap[section->connectTo()])
                checked = "checked";
            checkBoxString = "<div class=\"spec\"><h3><input type=\"checkbox\" "
                    "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" " +
                    checked + " name=\"" + section->connectTo() +
                    "\" value=\"" + section->connectTo() + "Value\" />" +
                    section->heading() + "</h3></div>\n";
            customLegendString += checkBoxString;
        }

        customLegendString += "<table border=\"0\" cellspacing=\"0\">\n";

        foreach (const GeoSceneItem *item, section->items()) {
            QString path;
            int pixmapWidth = 24;
            int pixmapHeight = 12;
            if (!item->icon()->pixmap().isEmpty()) {
                path = MarbleDirs::path( item->icon()->pixmap() );
                const QPixmap oncePixmap(path);
                pixmapWidth = oncePixmap.width();
                pixmapHeight = oncePixmap.height();
            } else {
                // Tiny hack ;)
                // There is <img src="%path%" />
                // We will have <img src="" style="display: none;" />
                path = "\" style=\"display: none;";
            }
            QColor color = item->icon()->color();
            QString styleDiv = "";
            QString styleTd = "";
            if (color != Qt::transparent) {
                styleDiv = "width: " + QString::number(pixmapWidth) + "px; height: " +
                                    QString::number(pixmapHeight) + "px; background-color: "
                        + color.name() + ";";
                styleTd = "width: " + QString::number(pixmapWidth) + "px; height: " +
                        QString::number(pixmapHeight) + "px;";

            } else {
                styleTd = "width: " + QString::number(pixmapWidth) + "px; height: " +
                        QString::number(pixmapHeight) + "px;";
                styleDiv = styleTd;
            }
            QString src  =  "file://" + path;

            QString html = ""
                    "   <tr>\n"
                    "       <td class=\"icon\" style=\"" + styleTd + "\">\n"
                    "           <div style=\"" + styleDiv + "\">\n"
                    "               <img src=\"" + src + "\" />\n"
                    "           </div>\n"
                    "       </td>\n"
                    "       <td class=\"text\" style=\"padding-left: 5px;\">\n"
                    "           <p>" + item->text().trimmed() + "</p>\n"
                    "       </td>\n"
                    "   </tr>\n";
            customLegendString += html;
        }

        customLegendString += "</table>\n";
    }

    return customLegendString;
}

void MarbleLegendBrowser::setCheckedProperty( const QString& name, bool checked )
{
    QWebElement box = page()->mainFrame()->findFirstElement("input[name="+name+"]");
    if (!box.isNull()) {
        if (checked != d->m_checkBoxMap[name]) {
            d->m_checkBoxMap[name] = checked;
            emit toggledShowProperty( name, checked );
        }
    }

    update();
}

}

#include "MarbleLegendBrowser.moc"
