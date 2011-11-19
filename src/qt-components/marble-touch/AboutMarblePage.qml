// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0

/*
 * Page for geocaching activity.
 */
Page {
    id: geocachingActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    Flickable {
        id: pageContent
        anchors.fill: parent
        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 20
            spacing: 12

            Image {
                source: "qrc:/../../../data/svg/marble-logo.svg"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: "Find your way and explore the world with Marble"
                platformStyle: LabelStyle { fontPixelSize: 20 }
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: "<p>This is Marble Touch <b>version 1.2.80 (incomplete alpha)</b>. Marble is a Virtual Globe and World Atlas. It is part of the KDE Software Compilation and distributed under the terms of the LGPL, Version 2. Marble is Open Source. We believe in free data, free maps and free source code. Please visit the <a href=\"http://edu.kde.org/marble\">Marble website</a> for further information.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignJustify
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: "<p>&copy; 2007-2011 by the authors of Marble Virtual Globe. Please report bugs and file feature requests at <a href=\"https://bugs.kde.org/enter_bug.cgi?product=marble&rep_platform=Meego/Harmattan\">bugs.kde.org</a>. We're looking forward to your feedback! You can reach the developers of the Marble project at <a href=\"mailto:\"marble-devel@kde.org\">marble-devel@kde.org</a>.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: "Authors"
                platformStyle: LabelStyle { fontPixelSize: 20 }
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: "<p><i>Active Development Team of Marble</i>: Torsten Rahn (Developer and Original Author), Bernhard Beschow (WMS Support, Mobile, Performance), Thibaut Gridel (Geodata), Jens-Michael Hoffmann (OpenStreetMap Support, Download Management), Florian Eßer (Elevation Profile), Wes Hardaker (Amateur Radio Support), Bastian Holst (Online Services Support), Guillaume Martres (Satellites), Friedrich W. H. Kossebau (Plasma Integration, Bugfixes), Dennis Nienhüser (Routing, Navigation, Mobile), Niko Sams (Routing, Elevation Profile), Patrick Spendrin (KML and Windows Support), Eckhart Wörner (Bugfixes).</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignJustify
            }

            Label {
                text: "<p><i>Developers</i>: Médéric Boquien (Astronomical Observatories), Harshit Jain (Planet Filter, Bugfixes), Andrew Manson (Proxy Support), Pino Toscano (Network plugins), Henry de Valence (Marble Runners, World-Clock Plasmoid), Magnus Valle (Historical Maps), Inge Wallin (Original Co-Maintainer).</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p><i>Development and Patches:</i>: Simon Schmeisser, Claudiu Covaci, David Roberts, Nikolas Zimmermann, Jan Becker, Stefan Asserhäll, Laurent Montel, Prashanth Udupa, Anne-Marie Mahfouf, Josef Spillner, Frerich Raabe, Frederik Gladhorn, Fredrik Höglund, Albert Astals Cid, Thomas Zander, Joseph Wenninger, Kris Thomsen, Daniel Molkentin.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p><i>Platforms and Distributions</i>: Tim Sutton, Christian Ehrlicher, Ralf Habacker, Steffen Joeris, Marcus Czeslinski, Marcus D. Hanwell, Chitlesh Goorah, Sebastian Wiedenroth, Christophe Leske.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p><i>Artwork</i>: Nuno Pinheiro, Torsten Rahn.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p><i>Various Suggestions and Testing</i>: Stefan Jordan, Robert Scott, Lubos Petrovic, Benoit Sigoure, Martin Konold, Matthias Welwarsky, Rainer Endres, Luis Silva, Ralf Gesellensetter, Tim Alder.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p><i>Student programs (GSOC, GCI, SOCIS)</i>: Guillaume Martres, Utku Aydın, Daniel Marth, Cezar Mocan, Furkan Üzümcü, Konstantin Oblaukhov, Daniel Marth, Gaurav Gupta, Harshit Jain, Siddharth Srivastava, Andrew Manson, Bastian Holst. Patrick Spendrin, Shashank Singh, Carlos Licea, Andrew Manson, Murad Tagirov. And of course we'd like to thank the people at Google and ESA for making these projects possible.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p>We'd especially like to thank John Layt who provided an important source of inspiration by creating Marble's predecessor \"Kartographer\".</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "Data"
                platformStyle: LabelStyle { fontPixelSize: 20 }
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: "<p>Maps: Blue Marble Next Generation (500 m / pixel). NASA Goddard Space Flight Center Earth Observatory http://earthobservatory.nasa.gov/Newsroom/BlueMarble/. Earth's City Lights. Data courtesy Marc Imhoff of NASA GSFC and Christopher Elvidge of NOAA NGDC. Image by Craig Mayhew and Robert Simmon, NASA GSFC. Shuttle Radar Topography Mission (SRTM30, 1 km / pixel ). NASA Jet Propulsion Laboratory http://www2.jpl.nasa.gov/srtm/. Micro World Data Bank in Polygons (\"MWDB-POLY / MWDBII\"). CIA ; Global Associates, Ltd.; Fred Pospeschil and Antonio Rivera. Temperature and Precipitation Maps (July and December). A combination of two datasets: Legates, D.R. and Willmott, C.J. 1989. Average Monthly Surface Air Temperature and Precipitation. Digital Raster Data on a .5 degree Geographic (lat/long) 361x721 grid (centroid-registered on .5 degree meridians). Boulder CO: National Center for Atmospheric Research. http://www.ngdc.noaa.gov/ecosys/cdroms/ged_iia/datasets/a04/lw.htm. CRU CL 2.0: New, M., Lister, D., Hulme, M. and Makin, I., 2002: A high-resolution data set of surface climate over global land areas. Climate Research 21.http://www.cru.uea.ac.uk/cru/data/hrg.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p>Street Maps. OpenStreetMap. The street maps used in Marble via download are provided by the OpenStreetMap Project (\"OSM\"). OSM is an open community which creates free editable maps. License: OpenStreetMap data can be used freely under the terms of the Creative Commons Attribution-ShareAlike 2.0 license. OpenStreetMap Icons. Some placemark icons are taken from SJJB Management. These icons can be used freely under the terms of the CC0 1.0 Universal license. OpenRouteService. The routes used in Marble via download are provided by the OpenRouteService Project (\"ORS\"). License: OpenRouteService data can be used freely under the terms of the Creative Commons Attribution-ShareAlike 2.0 license.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p>Cities and Locations. World Gazetteer. Stefan Helders http://www.world-gazetteer.com. Geonames.org. http://www.geonames.org/License: Geonames.org data can be used freely under the terms of the Creative Commons Attribution 3.0 license. Czech Statistical Office. Public database http://www.czso.cz.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p>Flags. Flags of the World. The flags were taken from Wikipedia (http://www.wikipedia.org) which in turn took a subset from http://www.openclipart.org and reworked them. All flags are under the public domain (see comments inside the svg files).</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: "<p>Stars. The Bright Star Catalogue. 5th Revised Ed. (Preliminary Version) Hoffleit D., Warren Jr W.H., Astronomical Data Center, NSSDC/ADC (1991) http://adc.gsfc.nasa.gov.</p>"
                platformStyle: LabelStyle { fontPixelSize: 16 }
                width: parent.width
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }

    ScrollDecorator {
        flickableItem: pageContent
    }
}
