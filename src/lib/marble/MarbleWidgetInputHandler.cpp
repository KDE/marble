// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#include "MarbleWidgetInputHandler.h"

#include <QRubberBand>
#include <QToolTip>
#include <QTimer>
#include <QKeyEvent>

#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "AbstractDataPluginItem.h"
#include "MarbleWidgetPopupMenu.h"
#include "PopupLayer.h"
#include "RenderPlugin.h"
#include "RoutingLayer.h"

namespace Marble
{

class MarbleWidgetInputHandlerPrivate
{
    class MarbleWidgetSelectionRubber : public AbstractSelectionRubber
    {
        public:
            explicit MarbleWidgetSelectionRubber(MarbleWidget *widget)
                : m_rubberBand(QRubberBand::Rectangle, widget)
            {
                m_rubberBand.hide();
            }

            void show() { m_rubberBand.show(); }
            void hide() { m_rubberBand.hide(); }
            bool isVisible() const { return m_rubberBand.isVisible(); }
            const QRect &geometry() const { return m_rubberBand.geometry(); }
            void setGeometry(const QRect &geometry) { m_rubberBand.setGeometry(geometry); }

        private:
            QRubberBand m_rubberBand;
    };

    public:
        MarbleWidgetInputHandlerPrivate(MarbleWidgetInputHandler *handler, MarbleWidget *widget)
            : m_inputHandler(handler)
            ,m_marbleWidget(widget)
            ,m_selectionRubber(widget)
            ,m_debugModeEnabled(false)
        {         
            foreach(RenderPlugin *renderPlugin, widget->renderPlugins())
            {
                if(renderPlugin->isInitialized())
                {
                    installPluginEventFilter(renderPlugin);
                }
            }
            m_marbleWidget->grabGesture(Qt::PinchGesture);
        }

        void setCursor(const QCursor &cursor)
        {
            m_marbleWidget->setCursor(cursor);
        }

        bool layersEventFilter(QObject *o, QEvent *e)
        {   //FIXME - this should go up in hierarchy to MarbleInputHandler
            if (m_marbleWidget->popupLayer()->eventFilter(o, e))
            {
                return true;
            }

            if (m_marbleWidget->routingLayer()->eventFilter(o, e))
            {
                return true;
            }

            return false;
        }

        void installPluginEventFilter(RenderPlugin *renderPlugin)
        {
            m_marbleWidget->installEventFilter(renderPlugin);
        }

        MarbleWidgetInputHandler *m_inputHandler;
        MarbleWidget *m_marbleWidget;
        MarbleWidgetSelectionRubber m_selectionRubber;
        bool m_debugModeEnabled;
};


void MarbleWidgetInputHandler::setCursor(const QCursor &cursor)
{
    d->setCursor(cursor);
}

bool MarbleWidgetInputHandler::handleKeyPress(QKeyEvent *event)
{
    if (d->m_debugModeEnabled) {
        switch(event->key()) {
        case Qt::Key_I:
            MarbleDebug::setEnabled(!MarbleDebug::isEnabled());
            break;
        case Qt::Key_R:
            d->m_marbleWidget->setShowRuntimeTrace(!d->m_marbleWidget->showRuntimeTrace());
            break;
        case Qt::Key_O:
            d->m_marbleWidget->setShowDebugPlacemarks(!d->m_marbleWidget->showDebugPlacemarks());
            break;
        case Qt::Key_P:
            d->m_marbleWidget->setShowDebugPolygons(!d->m_marbleWidget->showDebugPolygons());
            break;
        case Qt::Key_B:
            d->m_marbleWidget->setShowDebugBatchRender(!d->m_marbleWidget->showDebugBatchRender());
            break;
        }
    }
    return MarbleDefaultInputHandler::handleKeyPress(event);
}

AbstractSelectionRubber *MarbleWidgetInputHandler::selectionRubber()
{
    return &d->m_selectionRubber;
}

bool MarbleWidgetInputHandler::layersEventFilter(QObject *o, QEvent *e)
{
    return d->layersEventFilter(o, e);
}

void MarbleWidgetInputHandler::installPluginEventFilter(RenderPlugin *renderPlugin)
{
    d->installPluginEventFilter(renderPlugin);
}

MarbleWidgetInputHandler::MarbleWidgetInputHandler(MarbleAbstractPresenter *marblePresenter, MarbleWidget *widget)
    : MarbleDefaultInputHandler(marblePresenter)
    ,d(new MarbleWidgetInputHandlerPrivate(this, widget))
{
}

void MarbleWidgetInputHandler::setDebugModeEnabled(bool enabled)
{
    d->m_debugModeEnabled = enabled;
}

//FIXME - these should be moved to superclass and popupMenu should be abstracted in MarbleAbstractPresenter
void MarbleWidgetInputHandler::showLmbMenu(int x, int y)
{
    if (isMouseButtonPopupEnabled(Qt::LeftButton))
    {
        d->m_marbleWidget->popupMenu()->showLmbMenu(x, y);
        toolTipTimer()->stop();
    }
}

void MarbleWidgetInputHandler::showRmbMenu(int x, int y)
{
    if (isMouseButtonPopupEnabled(Qt::RightButton))
    {
        d->m_marbleWidget->popupMenu()->showRmbMenu(x, y);
    }
}

void MarbleWidgetInputHandler::openItemToolTip()
{
    if (!lastToolTipItem().isNull())
    {
        QToolTip::showText(d->m_marbleWidget->mapToGlobal(toolTipPosition()),
                            lastToolTipItem()->toolTip(),
                            d->m_marbleWidget,
                            lastToolTipItem()->containsRect(toolTipPosition()).toRect());
    }
}

}

#include "moc_MarbleWidgetInputHandler.cpp"
