// Copyright 2008 Henry de Valence
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


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
	void init();
        void paintInterface(QPainter *painter, 
	                    const QStyleOptionGraphicsItem *option,
                            const QRect& contentsRect);
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


