#!env python
#
#    Copyright 2008 Simon Edwards <simon@simonzone.com>
#
#    This file is part of the Marble Virtual Globe.
#
#    This library is free software you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation either
#    version 2 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.
#
#    You should have received a copy of the GNU Library General Public License
#    aint with this library see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#    Boston, MA 02110-1301, USA.
#
# A very simple example of how to use the marble widget.

from PyQt4.QtCore import *
from PyKDE4.kdeui import *
from PyKDE4.kdecore import *
from PyKDE4.marble import *
import sys

class MainWin (KMainWindow):
    def __init__ (self, *args):
        KMainWindow.__init__ (self)
        self.resize(640, 480)

        self.marble = Marble.MarbleWidget(self)
        self.marble.setMapThemeId("earth/bluemarble/bluemarble.dgml")
        self.setCentralWidget(self.marble)
        self.marble.centerOn(5.8333, 51.8333)
        self.marble.zoomView(2000)

def main():
    appName     = "simple_marble"
    catalog     = ""
    programName = ki18n ("default")
    version     = "1.0"
    description = ki18n ("Simple Python Marble Example")
    license     = KAboutData.License_GPL
    copyright   = ki18n ("(c) 2008 Simon Edwards")
    text        = ki18n ("none")
    homePage    = "www.simonzone.com"
    bugEmail    = "simon@simonzone.com"

    aboutData   = KAboutData (appName, catalog, programName, version, description,
                              license, copyright, text, homePage, bugEmail)

    aboutData.addAuthor (ki18n("Simon Edwards"), ki18n("Developer"))
    
    KCmdLineArgs.init(sys.argv, aboutData)
    
    app = KApplication()
    mainWindow = MainWin(None, "main window")
    mainWindow.show()
    app.connect(app, SIGNAL ("lastWindowClosed ()"), app.quit)
    app.exec_()

main()
