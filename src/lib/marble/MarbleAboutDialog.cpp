/* This file is part of the KDE project
 *
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "MarbleAboutDialog.h"
#include "ui_MarbleAboutDialog.h"

#include <QFile>
#include <QTextFrame>
#include <QTextStream>
#include <QPixmap>

#include "MarbleGlobal.h"
#include "MarbleDirs.h"

namespace Marble
{

class MarbleAboutDialogPrivate
{
public: 
    MarbleAboutDialogPrivate();

    void loadPageContents( int idx );

    Ui::MarbleAboutDialog  uiWidget;

    bool authorsLoaded;
    bool dataLoaded;
    bool licenseLoaded;
};

MarbleAboutDialogPrivate::MarbleAboutDialogPrivate() :
    authorsLoaded( false ),
    dataLoaded( false ),
    licenseLoaded( false )
{
}

MarbleAboutDialog::MarbleAboutDialog(QWidget *parent)
    : QDialog( parent ),
      d( new MarbleAboutDialogPrivate )
{
    d->uiWidget.setupUi( this );

    if( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        d->uiWidget.m_pMarbleTitleLabel->hide();
        d->uiWidget.m_pMarbleVersionLabel->hide();
        d->uiWidget.m_pMarbleLogoLabel->hide();
    }
    else {
        d->uiWidget.m_pMarbleLogoLabel->setPixmap( 
                QPixmap(MarbleDirs::path(QStringLiteral("svg/marble-logo-72dpi.png"))));
    }

    QString const applicationTitle = QObject::tr( "Marble Virtual Globe" );
    setApplicationTitle( applicationTitle );

    connect( d->uiWidget.tabWidget, SIGNAL(currentChanged(int)), 
             this, SLOT(loadPageContents(int)) );
}

MarbleAboutDialog::~MarbleAboutDialog()
{
   delete d;
}

void MarbleAboutDialogPrivate::loadPageContents( int idx )
{
    if ( idx == 1 && !authorsLoaded )
    {
        authorsLoaded = true;
        QTextBrowser *const browser = uiWidget.m_pMarbleAuthorsBrowser;
        browser->setHtml( QObject::tr("<b>Active Development Team of Marble</b>")+
        QObject::tr("<p>Torsten Rahn <a href=\"mailto:rahn@kde.org\">rahn@kde.org</a><br />"
           "<i>Developer and Original Author</i></p>")+
        QObject::tr("<p>Dennis Nienh&uuml;ser <a href=\"mailto:nienhueser@kde.org\">nienhueser@kde.org</a><br />"
           "<i>Routing, Navigation, Mobile</i></p>")+
        QObject::tr("<p>Bernhard Beschow <a href=\"mailto:bbeschow@cs.tu-berlin.de\">bbeschow@cs.tu-berlin.de</a><br />"
           "<i>WMS Support, Mobile, Performance</i></p>")+
        QObject::tr("<p>Friedrich W. H. Kossebau, <a href=\"mailto:kossebau@kde.org\">kossebau@kde.org</a><br />"
           "<i>Plasma Integration, Bugfixes</i></p>")+
        QObject::tr("<p>Thibaut Gridel <a href=\"mailto:tgridel@free.fr\">tgridel@free.fr</a><br />"
           "<i>Geodata</i></p>")+
        QObject::tr("<p>Jens-Michael Hoffmann <a href=\"mailto:jensmh@gmx.de\">jensmh@gmx.de</a><br />"
           "<i>OpenStreetMap Support, Download Management</i></p>")+
        QObject::tr("<p>Florian E&szlig;er <a href=\"mailto:f.esser@rwth-aachen.de\">f.esser@rwth-aachen.de</a><br />"
           "<i>Elevation Profile</i></p>")+
        QObject::tr("<p>Wes Hardaker <a href=\"mailto:marble@hardakers.net\">marble@hardakers.net</a><br />"
           "<i>Amateur Radio Support</i></p>")+
        QObject::tr("<p>Bastian Holst, <a href=\"mailto:bastianholst@gmx.de\">bastianholst@gmx.de</a><br />"
           "<i>Online Services Support</i></p>")+
        QObject::tr("<p>Guillaume Martres, <a href=\"mailto:smarter@ubuntu.com\">smarter@ubuntu.com</a><br />"
           "<i>Satellites</i></p>")+
        QObject::tr("<p>Ren&#xE9; K&#xFC;ttner, <a href=\"mailto:rene@bitkanal.net\">rene@bitkanal.net</a><br />"
           "<i>Planetary Satellites</i></p>")+
        QObject::tr("<p>Niko Sams <a href=\"mailto:niko.sams@gmail.com\">niko.sams@gmail.com</a><br />"
           "<i>Routing, Elevation Profile</i></p>")+
        QObject::tr("<p>Patrick Spendrin <a href=\"pspendrin@gmail.com\">pspendrin@gmail.com</a><br />"
           "<i>KML and Windows Support</i></p>")+
        QObject::tr("<p>Eckhart W&ouml;rner <a href=\"mailto:kde@ewsoftware.de\">kde@ewsoftware.de</a><br />"
           "<i>Bugfixes</i></p>")+
        QObject::tr("<b>Developers</b>")+
        QObject::tr("<p>M&eacute;d&eacute;ric Boquien <a href=\"mailto:mboquien@free.fr\">mboquien@free.fr</a><br />"
           "<i>Astronomical Observatories</i></p>")+
        QObject::tr("<p>Harshit Jain <a href=\"mailto:sonu.itbhu@googlemail.com\">sonu.itbhu@googlemail.com</a><br />"
           "<i>Planet Filter, Bugfixes</i></p>")+
        QObject::tr("<p>Andrew Manson <a href=\"mailto:g.real.ate@gmail.com\">g.real.ate@gmail.com</a><br />"
           "<i>Proxy Support</i></p>")+
        QObject::tr("<p>Pino Toscano <a href=\"mailto:pino@kde.org\">pino@kde.org</a><br />"
           "<i>Network plugins</i></p>")+
        QObject::tr("<p>Henry de Valence <a href=\"mailto:hdevalence@gmail.com\">hdevalence@gmail.com</a><br />"
           "<i>Marble Runners, World-Clock Plasmoid</i></p>")+
        QObject::tr("<p>Magnus Valle<br /><i>Historical Maps</i></p>")+
        QObject::tr("<p>Inge Wallin <a href=\"mailto:inge@lysator.liu.se\">inge@lysator.liu.se</a><br />"
           "<i>Original Co-Maintainer</i></p>")+
        QObject::tr("<p><i>Development &amp; Patches:</i> Simon Schmeisser, Claudiu Covaci, David Roberts, Nikolas Zimmermann, Jan Becker, "
           "Stefan Asserh&auml;ll, Laurent Montel, Prashanth Udupa, Anne-Marie Mahfouf, Josef Spillner, Frerich Raabe, "
           "Frederik Gladhorn, Fredrik H&ouml;glund, Albert Astals Cid, Thomas Zander, Joseph Wenninger, Kris Thomsen, "
           "Daniel Molkentin  </p>")+
        QObject::tr("<p><i>Platforms &amp; Distributions:</i> "
           "Tim Sutton, Christian Ehrlicher, Ralf Habacker, Steffen Joeris, Marcus Czeslinski, Marcus D. Hanwell, Chitlesh Goorah, Sebastian Wiedenroth, Christophe Leske</p>")+
        QObject::tr("<p><i>Artwork:</i> Nuno Pinheiro, Torsten Rahn</p>")+
        QObject::tr("<b>Join us</b>")+
        QObject::tr("<p>You can reach the developers of the Marble Project at <a href=\"mailto:marble-devel@kde.org\">marble-devel@kde.org</a></p>")+
        QObject::tr("<b>ESA - Summer of Code in Space</b>")+
        QObject::tr("<p> The Marble Team would like to thank its members who participated "
           "in ESA SoCiS for their successful work on Marble:</p>")+
        QObject::tr("<p><b>2016</b></p>")+
        QObject::tr("<p>Judit Bartha, <a href=\"mailto:bartha.m.judit@gmail.com\"> bartha.m.judit@gmail.com </a><br />"
                                                                 "<i>Project: Improving Marble's Satellite maps using Sentinel-2 mission data</i></p>")+
        QObject::tr("<p><b>2015</b></p>")+
        QObject::tr("<p>Ana Badescu, <a href=\"mailto:anabee.emacs@gmail.com\"> anabee.emacs@gmail.com </a><br />"
                                                                 "<i>Project: Integrate data provided by the Sentinel missions</i></p>")+
        QObject::tr("<p><b>2014</b></p>")+
        QObject::tr("<p>G&#xE1;bor P&#xE9;terffy, <a href=\"mailto:peterffy95@gmail.com\"> peterffy95@gmail.com </a><br />"
                                                                 "<i>Project: Panoramic Picture Support for celestial bodies in Marble   </i></p>")+
        QObject::tr("<p><b>2013</b></p>")+
        QObject::tr("<p>Marek Hakala, <a href=\"mailto:hakala.marek@gmail.com\">hakala.marek@gmail.com</a><br />"
                                               "<i>Project: Displaying Solar / Lunar Eclipses in Marble </i></p>")+
        QObject::tr("<p><b>2012</b></p>")+
        QObject::tr("<p>Ren&#xE9; K&#xFC;ttner, <a href=\"mailto:rene@bitkanal.net\">rene@bitkanal.net</a><br />"
           "<i>Project: Visualization of planetary satellites</i></p>")+
        QObject::tr("<p><b>2011</b></p>")+
        QObject::tr("<p>Guillaume Martres, <a href=\"mailto:smarter@ubuntu.com\">smarter@ubuntu.com</a><br />"
           "<i>Project: Visualization of Satellite Orbits</i></p>")+

        QObject::tr("<b>Google Code-in</b>")+
        QObject::tr("<p> The Marble Team would like to thank its members who participated "
           "in the Google Code-in for their successful work on Marble:</p>")+
        QObject::tr("<p><b>2014</b></p>")+
        QObject::tr( "Ilya Kowalewski <a href=\"mailto:illya.kovalevskyy@gmail.com\">illya.kovalevskyy@gmail.com</a><br />" ) +
        QObject::tr( "Mikhail Ivchenko <a href=\"mailto:ematirov@gmail.com\">ematirov@gmail.com</a><br />" ) +
        QObject::tr( "Sergey Popov <a href=\"mailto:sergobot256@gmail.com\">sergobot256@gmail.com</a><br />" ) +
        QObject::tr( "Daniel Pastushchak <a href=\"mailto:danikpastushchak90@gmail.com\">danikpastushchak90@gmail.com</a><br />" ) +

        QObject::tr("<p><b>2013</b></p>")+
        QObject::tr( "Ilya Kowalewski <a href=\"mailto:illya.kovalevskyy@gmail.com\">illya.kovalevskyy@gmail.com</a><br />" ) +
        QObject::tr( "Mikhail Ivchenko <a href=\"mailto:ematirov@gmail.com\">ematirov@gmail.com</a><br />" ) +
        QObject::tr( "Levente Kurusa <a href=\"mailto:levex@linux.com\">levex@linux.com</a><br />" ) +
        QObject::tr( "Benjamin Kaiser <a href=\"mailto:benjaminjkaiser@gmail.com\">benjaminjkaiser@gmail.com</a><br />" ) +
        QObject::tr( "<p><b>2012</b></p>" ) +
        QObject::tr( "Ilya Kowalewski <a href=\"mailto:illya.kovalevskyy@gmail.com\">illya.kovalevskyy@gmail.com</a><br />" ) +
        QObject::tr( "Mohammed Nafees <a href=\"mailto:nafees.technocool@gmail.com\">nafees.technocool@gmail.com</a></p><br />" ) +
        QObject::tr( "Mayank Madan <a href=\"mailto:maddiemadan@gmail.com\">maddiemadan@gmail.com</a><br />" ) +
        QObject::tr( "Timothy Lanzi <a href=\"mailto:trlanzi@gmail.com\">trlanzi@gmail.com</a></p>" ) +
        QObject::tr( "<p><b>2011</b></p>" ) +
        QObject::tr( "<p>Utku Ayd&#x131;n <a href=\"mailto:utkuaydin34@gmail.com\">utkuaydin34@gmail.com</a><br />" ) +
        QObject::tr( "Daniel Marth <a href=\"mailto:danielmarth@gmx.at\">danielmarth@gmx.at</a><br />" ) +
        QObject::tr( "Cezar Mocan <a href=\"mailto:mocancezar@gmail.com\">mocancezar@gmail.com</a><br />" ) +
        QObject::tr( "Furkan &Uuml;z&uuml;mc&uuml; <a href=\"mailto:furkanuzumcu@gmail.com\">furkanuzumcu@gmail.com</a></p>" ) +

        QObject::tr("<b>Google Summer of Code</b>")+
        QObject::tr("<p> The Marble Team would like to thank its members who participated "
           "in the Google Summer of Code for their successful work on Marble:</p>")+
        QObject::tr( "<p><b>2017</b></p>" ) +
        QObject::tr( "<p>Mohammed Nafees <a href=\"mailto:nafees.technocool@gmail.com\">nafees.technocool@gmail.com</a><br />"
           "<i>Project: Marble Indoor Maps</i></p>" ) +
        QObject::tr( "<p>Judit Bartha <a href=\"mailto:bartha.m.judit@gmail.com\">bartha.m.judit@gmail.com</a><br />"
           "<i>Project: Marble Material Maps</i></p>" ) +
        QObject::tr( "<p><b>2016</b></p>" ) +
        QObject::tr( "<p>Dávid Kolozsvári <a href=\"freedawson@gmail.com\">freedawson@gmail.com</a><br />"
           "<i>Project: Fluent graphics across every tile level in Marble's OSM vector map</i></p>" ) +
        QObject::tr( "<p>Akshat Tandon <a href=\"akshat.tandon@research.iiit.ac.in\">akshat.tandon@research.iiit.ac.in</a><br />"
           "<i>Project: Support for medium and low tile levels in the OSM Vector Map of Marble</i></p>" ) +
        QObject::tr( "<p><b>2015</b></p>" ) +
        QObject::tr( "<p>Dávid Kolozsvári <a href=\"freedawson@gmail.com\">freedawson@gmail.com</a><br />"
           "<i>Project: Improve Marble's OSM vector rendering support</i></p>" ) +
        QObject::tr( "<p>Gábor Péterffy <a href=\"gabor.peterffy@gmail.com\">gabor.peterffy@gmail.com</a><br />"
           "<i>Project: Porting Marble to Android platform</i></p>" ) +
        QObject::tr( "<p>Marius Valeriu Stanciu <a href=\"stanciumarius94@gmail.com\">stanciumarius94@gmail.com</a><br />"
           "<i>Project: Offering OpenStreetMap support and improving Edit Mode for Marble</i></p>" ) +
        QObject::tr( "<p><b>2014</b></p>" ) +
        QObject::tr( "<p>Abhinav Gangwar <a href=\"abhgang@gmail.com\">abhgang@gmail.com</a><br />"
           "<i>Project: Marble Game</i></p>" ) +
        QObject::tr( "<p>Calin Cruceru <a href=\"crucerucalincristian@gmail.com\">crucerucalincristian@gmail.com</a><br />"
           "<i>Project: Editing Mode for Polygons</i></p>" ) +
        QObject::tr( "<p>Sanjiban Bairagya <a href=\"sanjiban22393@gmail.com\">sanjiban22393@gmail.com</a><br />"
           "<i>Project: Interactive Tours </i></p>" ) +
        QObject::tr( "<p><b>2013</b></p>" ) +
        QObject::tr( "<p>Adrian Draghici <a href=\"draghici.adrian.b@gmail.com\">draghici.adrian.b@gmail.com</a><br />"
           "<i>Project: Marble KML Map Editor</i></p>" ) +
        QObject::tr( "<p>Andrei Duma <a href=\"andrei.duma.dorian@gmail.com\">andrei.duma.dorian@gmail.com</a><br />"
           "<i>Project: Marble meets ownCloud</i></p>" ) +
        QObject::tr( "<p>Utku Ayd&#x131;n <a href=\"utkuaydin34@gmail.com\">utkuaydin34@gmail.com</a><br />"
           "<i>Project: ownCloud storage and synchronization for Marble </i></p>" ) +
        QObject::tr( "<p>Ren&#xE9; K&#xFC;ttner <a href=\"rene@bitkanal.net\">rene@bitkanal.net</a><br />"
           "<i>Project: OpenGL SceneGraph</i></p>" ) +
        QObject::tr( "<p><b>2012</b></p>" ) +
        QObject::tr( "<p>Ander Pijoan <a href=\"ander.pijoan@deusto.es\">ander.pijoan@deusto.es</a><br />"
            "<i>Project: OpenStreetMap Vector Rendering</i></p>" ) +
        QObject::tr( "<p>Cezar Mocan <a href=\"mocancezar@gmail.com\">mocancezar@gmail.com</a><br />"
            "<i>Project: Natural Earth Vector Map</i></p>" ) +
        QObject::tr( "<p>Bernhard Beschow <a href=\"bbeschow@cs.tu-berlin.de\">bbeschow@cs.tu-berlin.de</a><br />"
            "<i>Project: OpenGL Mode for Marble</i></p>" ) +
        QObject::tr( "<p><b>2011</b></p>" ) +
        QObject::tr( "<p>Konstantin Oblaukhov <a href=\"oblaukhov.konstantin@gmail.com\">oblaukhov.konstantin@gmail.com</a><br />"
            "<i>Project: OpenStreetMap Vector Rendering</i></p>" ) +
        QObject::tr( "<p>Daniel Marth <a href=\"danielmarth@gmx.at\">danielmarth@gmx.at</a><br />"
            "<i>Project: Marble Touch on MeeGo</i></p>" ) +
        QObject::tr( "<p><b>2010</b></p>" ) +
        QObject::tr( "<p>Gaurav Gupta <a href=\"mailto:1989.gaurav@gmail.com\">1989.gaurav@gmail.com</a><br />"
            "<i>Project: Bookmarks</i></p>" ) +
        QObject::tr( "<p>Harshit Jain <a href=\"mailto:hjain.itbhu@gmail.com\">hjain.itbhu@gmail.com</a><br />"
            "<i>Project: Time Support</i></p>" ) +
        QObject::tr( "<p>Siddharth Srivastava <a href=\"mailto:akssps011@gmail.com\">akssps011@gmail.com</a><br />"
            "<i>Project: Turn-by-turn Navigation</i></p>" ) +
        QObject::tr( "<p><b>2009</b></p>" ) +
        QObject::tr( "<p>Andrew Manson <a href=\"mailto:g.real.ate@gmail.com\">g.real.ate@gmail.com</a><br />"
            "<i>Project: OSM Annotation</i></p>" ) +
        QObject::tr( "<p>Bastian Holst <a href=\"mailto:bastianholst@gmx.de\">bastianholst@gmx.de</a><br />"
            "<i>Project: Online Services</i></p>" ) +
        QObject::tr("<p><b>2008</b></p>")+
        QObject::tr("<p>Patrick Spendrin <a href=\"pspendrin@gmail.com\">pspendrin@gmail.com</a><br />"
           "<i>Project: Vector Tiles for Marble</i></p>")+
        QObject::tr("<p>Shashank Singh <a href=\"mailto:shashank.personal@gmail.com\">shashank.personal@gmail.com</a><br />"
           "<i>Project: Panoramio / Wikipedia -photo support for Marble</i></p>")+
        QObject::tr("<b>2007</b>")+
        QObject::tr("<p>Carlos Licea <a href=\"mailto:carlos.licea@kdemail.net\">carlos.licea@kdemail.net</a><br />"
           "<i>Project: Equirectangular Projection (\"Flat Map\")</i></p>")+
        QObject::tr("<p>Andrew Manson <a href=\"mailto:g.real.ate@gmail.com\">g.real.ate@gmail.com</a><br />"
           "<i>Project: GPS Support for Marble</i></p>")+
        QObject::tr("<p>Murad Tagirov <a href=\"mailto:tmurad@gmail.com\">tmurad@gmail.com</a><br />"
           "<i>Project: KML Support for Marble</i></p>")+
        QObject::tr("<p>... and of course we'd like to thank the people at Google Inc. for making these projects possible.</p>")+
        QObject::tr("<b>Credits</b>")+
        QObject::tr("<p><i>Various Suggestions &amp; Testing:</i> Stefan Jordan, Robert Scott, Lubos Petrovic, Benoit Sigoure, "
           "Martin Konold, Matthias Welwarsky, Rainer Endres, Luis Silva, Ralf Gesellensetter, Tim Alder</p>")+
        QObject::tr("<p> We'd especially like to thank John Layt who provided an important source of inspiration "
           "by creating Marble's predecessor \"Kartographer\".</p>"));
        QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
        format.setMargin(12);
        browser->document()->rootFrame()->setFrameFormat( format );
    }

    if ( idx == 2 && !dataLoaded )
    {
        dataLoaded = true;
        QTextBrowser *const browser = uiWidget.m_pMarbleDataBrowser;
        browser->setHtml( QObject::tr("<b>Maps</b>")+
        QObject::tr("<p><i>Blue Marble Next Generation (500 m / pixel)</i><br />"
           "NASA Goddard Space Flight Center Earth Observatory "
           "<a href=\"http://earthobservatory.nasa.gov/Newsroom/BlueMarble/\">"
           "http://earthobservatory.nasa.gov/Newsroom/BlueMarble/</a></p>")+
        QObject::tr("<p><i>Earth's City Lights</i><br />"
           "Data courtesy Marc Imhoff of NASA GSFC and Christopher Elvidge of NOAA NGDC. "
           "Image by Craig Mayhew and Robert Simmon, NASA GSFC.</p>")+
        QObject::tr("<p><i>Shuttle Radar Topography Mission (SRTM30, 1 km / pixel )</i><br />"
           "NASA Jet Propulsion Laboratory <a href=\"http://www2.jpl.nasa.gov/srtm/\">"
           "http://www2.jpl.nasa.gov/srtm/</a></p>")+
        QObject::tr("<p><i>Micro World Data Bank in Polygons (\"MWDB-POLY / MWDBII\")</i><br />"
           "CIA ; Global Associates, Ltd.; Fred Pospeschil and Antonio Rivera</p>")+
        QObject::tr("<p><i>Temperature and Precipitation Maps (July and December)</i><br />"
           "A combination of two datasets:"
           "<ul>"
           "<li>Legates, D.R. and Willmott, C.J. 1989. Average Monthly Surface Air Temperature and "
           "Precipitation. Digital Raster Data on a .5 degree Geographic (lat/long) 361x721 grid "
           "(centroid-registered on .5 degree meridians). Boulder CO: National Center for Atmospheric Research. "
           "<a href=\"http://www.ngdc.noaa.gov/ecosys/cdroms/ged_iia/datasets/a04/lw.htm\">"
           "http://www.ngdc.noaa.gov/ecosys/cdroms/ged_iia/datasets/a04/lw.htm</a></li>"
           "<li>CRU CL 2.0: New, M., Lister, D., Hulme, M. and Makin, I., 2002: A high-resolution "
           "data set of surface climate over global land areas. Climate Research 21."
           "<a href=\"http://www.cru.uea.ac.uk/cru/data/hrg\">"
           "http://www.cru.uea.ac.uk/cru/data/hrg</a></li></ul></p>")+
        QObject::tr("<b>Street Map</b>")+
        QObject::tr("<p><i>OpenStreetMap</i><br />"
           "The street maps used in Marble via download are provided by the <a href=\""
           "http://www.openstreetmap.org\">OpenStreetMap</a> Project (\"OSM\"). "
           "OSM is an open community which creates free editable maps.<br />"
           "<i>License</i>: OpenStreetMap data can be used freely under the terms of the "
           "<a href=\"http://wiki.openstreetmap.org/index.php/OpenStreetMap_License\">"
           "Creative Commons Attribution-ShareAlike 2.0 license</a>.</p>")+
        QObject::tr( "<p><i>Icons</i><br />"
            "Some icons are taken from <a href=\"http://www.sjjb.co.uk/mapicons\">SJJB Management</a> and <a href=\"http://thenounproject.com\">NounProject</a>. "
            "These icons can be used freely under the terms of the "
            "<a href=\"http://creativecommons.org/publicdomain/zero/1.0/\">CC0 1.0 Universal</a> license.</p>") +
        QObject::tr( "<p>Some icons are taken from <a href=\"http://fortawesome.github.com/Font-Awesome\">Font Awesome</a>. "
            "These icons can be used freely under the terms of the "
            "<a href=\"http://creativecommons.org/licenses/by/3.0/\">CC BY 3.0</a> license.</p>") +
        QObject::tr("<p><i>OpenRouteService</i><br />"
           "Some of the routes used in Marble via download are provided by the <a href=\""
           "http://www.openrouteservice.org\">OpenRouteService</a> Project (\"ORS\"). <br />"
           "<i>License</i>: OpenRouteService data can be used freely under the terms of the "
           "<a href=\"http://wiki.openstreetmap.org/index.php/OpenStreetMap_License\">"
           "Creative Commons Attribution-ShareAlike 2.0 license</a>.</p>")+
        QObject::tr("<p><i>Open Source Routing Machine</i><br />"
           "Some of the routes used in Marble via download are provided by the <a href=\""
           "http://project-osrm.org\">Open Source Routing Machine</a> Project (\"OSRM\"). <br />"
           "<i>License</i>: Open Source Routing Machine data can be used freely under the terms of the "
           "<a href=\"http://wiki.openstreetmap.org/index.php/OpenStreetMap_License\">"
           "Creative Commons Attribution-ShareAlike 2.0 license</a>.</p>")+
        QObject::tr("<p><i>MapQuest</i><br />"
           "Some of the routes used in Marble via download are provided by <a href=\""
           "http://www.mapquest.com/\">MapQuest</a> and their Open Data Map APIs and Web Services.<br />"
           "Directions courtesy of MapQuest working on OpenStreetMap data that can be used freely under the terms of the "
           "<a href=\"http://wiki.openstreetmap.org/index.php/OpenStreetMap_License\">"
           "Creative Commons Attribution-ShareAlike 2.0 license</a>. Usage of the MapQuest routing "
           "service is subject to the <a href=\"http://info.mapquest.com/terms-of-use/\">MapQuest terms of use</a>.</p>")+
        QObject::tr("<b>Cities and Locations</b>")+
        QObject::tr("<p><i>World Gazetteer</i><br />Stefan Helders "
           "<a href=\"http://www.world-gazetteer.com\">http://www.world-gazetteer.com</a></p>")+
        QObject::tr("<p><i>Geonames.org</i><br />"
           "<a href=\"http://www.geonames.org/\">http://www.geonames.org/</a>"
           "<i>License</i>: Geonames.org data can be used freely under the terms of the "
           "<a href=\"http://creativecommons.org/licenses/by/3.0/\">Creative Commons Attribution 3.0 license</a>.</p>")+
        QObject::tr("<p><i>Czech Statistical Office</i><br />Public database "
           "<a href=\"http://www.czso.cz/eng/redakce.nsf/i/home\">http://www.czso.cz</a></p>")+
        QObject::tr("<b>Flags</b>")+
        QObject::tr("<p><i>Flags of the World</i><br />The flags were taken from Wikipedia "
           "(<a href=\"http://www.wikipedia.org\">http://www.wikipedia.org</a>) "
           "which in turn took a subset from <a href=\"http://www.openclipart.org\">"
           "http://www.openclipart.org</a> and reworked them. "
           "All flags are under the public domain (see comments inside the svg files).</p>")+
        QObject::tr("<b>Stars</b>")+
        QObject::tr("<p><i>The Bright Star Catalogue</i><br />5th Revised Ed. (Preliminary Version) " // krazy:exclude=spelling
           "Hoffleit D., Warren Jr W.H., Astronomical Data Center, NSSDC/ADC (1991)"
           "<a href=\"http://adc.gsfc.nasa.gov/adc-cgi/cat.pl?/catalogs/5/5050\"> "
           "http://adc.gsfc.nasa.gov</a></p>") );
        QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
        format.setMargin(12);
        browser->document()->rootFrame()->setFrameFormat( format );
    }

    if ( idx == 3 && !licenseLoaded )
    {
        licenseLoaded = true;
        QTextBrowser *const browser = uiWidget.m_pMarbleLicenseBrowser;
        const QString filename = MarbleDirs::path(QStringLiteral("LICENSE.txt"));
        if( !filename.isEmpty() )
        {
            QFile  f( filename );
            if( f.open( QIODevice::ReadOnly ) ) 
            {
                QTextStream ts( &f );
                browser->setText( ts.readAll() );
            }
            f.close();
        }
        QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
        format.setMargin(12);
        browser->document()->rootFrame()->setFrameFormat( format );
    }
}

void MarbleAboutDialog::setApplicationTitle( const QString &title )
{
    QString const titleHtml = QLatin1String("<b>") + title + QLatin1String("</b>");
    d->uiWidget.m_pMarbleTitleLabel->setText( titleHtml );
    QString const applicationVersion = tr( "Using Marble Library version %1" ).arg( MARBLE_VERSION_STRING );
    d->uiWidget.m_pMarbleVersionLabel->setText( applicationVersion );
    QTextBrowser* browser = d->uiWidget.m_pMarbleAboutBrowser;
    QString text;
    if( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        text = titleHtml + QLatin1String("<br />") + applicationVersion + QLatin1String("<br />");
    }
    browser->setHtml( text + tr("<br />(c) 2007-%1 by the authors of Marble Virtual Globe<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>").arg( 2015 ) );
    QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
    format.setMargin(12);
    browser->document()->rootFrame()->setFrameFormat( format );
}

void MarbleAboutDialog::setInitialTab( MarbleAboutDialog::Tab tab )
{
    d->uiWidget.tabWidget->setCurrentIndex( tab );
}

}

#include "moc_MarbleAboutDialog.cpp"
