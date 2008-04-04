//(C) Henry de Valence 2008
/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

#ifndef WORLDCLOCK_H
#define WORLDCLOCK_H

#include <Plasma/Applet>
#include "ui_worldclockConfig.h"

class MarbleMap;
class SunLocator;

// Define our plasma Applet
class WorldClock : public Plasma::Applet
{
    Q_OBJECT
    public:
        WorldClock(QObject *parent, const QVariantList &args);
        ~WorldClock();
        void paintInterface(QPainter *painter, 
	                    const QStyleOptionGraphicsItem *option,
                            const QRect& contentsRect);
        void init();
    public slots:
        void dataUpdated(const QString &name,
	                 const Plasma::DataEngine::Data &data);
	void showConfigurationInterface();
    protected slots:
        void configAccepted();
    private slots:
        void resizeMap();
    private:
        void connectToEngine();
        MarbleMap *m_map;
	SunLocator *m_sun;
	KDialog *m_configDialog;
	Ui::worldclockConfig ui;
};
 
K_EXPORT_PLASMA_APPLET(worldclock, WorldClock)
#endif

