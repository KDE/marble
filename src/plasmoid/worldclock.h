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
// License along with this library. If not, see <http://www.gnu.org/licenses/>.


#ifndef WORLDCLOCK_H
#define WORLDCLOCK_H


#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include "ui_worldclockConfig.h"


class MarbleMap;
class SunLocator;

class QGraphicsSceneHoverEvent;
class QStyleOptionGraphicsItem;
class QFont;
class QRect;
class QString;
class QPainter;
class QDateTime;
class QPointF;

class KTimeZone;

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
        void dataUpdated(const QString &source,
	                 const Plasma::DataEngine::Data &data);
	void createConfigurationInterface(KConfigDialog *parent);
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
        void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    protected slots:
        void configAccepted();
    private slots:
        //connected to geometryChanged()
        void resizeMap();
    private:
        void connectToEngine();

        //these are used for sizing & positioning text
        void recalculatePoints();
        void recalculateFonts();

        //for changing zones based on mouse position
        void setTz( QString newtz );
        QString getZone();

        //time in our selected timezone
        QDateTime m_time;
        //time in user's local zone
        QDateTime m_localtime;

        //the map
        MarbleMap *m_map;
	SunLocator *m_sun;

        //hover stuff
        bool m_isHovered;
        QPointF m_hover;

        //control preferences
        int m_timeDisplay;
        bool m_showFull;
        bool m_showDate;
        bool m_showSeconds;

        //map of locations and key for accessing it.
        QString m_locationkey;
        QMap<QString, KTimeZone> m_locations;

        //so we can check if the size has changed
        QRect m_lastRect;
        
        //Font sizing & positioning
        QFont m_timeFont;
        QFont m_dateFont;
        QFont m_locationFont;
        QHash<QString, QPoint> m_points; 

        //engine
        Plasma::DataEngine *m_timeEngine;

        //config gui
	Ui::worldclockConfig ui;
};
 
K_EXPORT_PLASMA_APPLET(worldclock, WorldClock)
#endif


