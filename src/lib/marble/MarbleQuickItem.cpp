//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#include <MarbleQuickItem.h>
#include <QPainter>
#include <QPaintDevice>
#include <QtMath>
#include <MarbleModel.h>
#include <MarbleMap.h>
#include <ViewportParams.h>
#include <GeoPainter.h>
#include <GeoDataLookAt.h>
#include <MarbleLocale.h>
#include <Planet.h>
#include <MarbleAbstractPresenter.h>

namespace Marble
{
    //TODO - just a test stub, integrate with MarbleWidgetInputHandler after refactoring it
    class MarbleQuickInputHandler : public QObject
    {
    public:

        MarbleQuickInputHandler(MarbleAbstractPresenter *marblePresenter) : m_marblePresenter(marblePresenter)
        {
        }

        bool eventFilter(QObject* object, QEvent* e)
        {
            if (e->type() == QEvent::KeyPress)
            {
                QKeyEvent *keyPressEvent = static_cast<QKeyEvent*>(e);
                return handleKeyPressEvent(keyPressEvent);
            }
            return QObject::eventFilter(object, e);
        }

        bool handleKeyPressEvent(QKeyEvent *event)
        {
            FlyToMode mode = Automatic;
            bool accept = true;
            switch (event->key())
            {
                case Qt::Key_Left:
                    m_marblePresenter->moveByStep(-1, 0, mode);
                    break;
                case Qt::Key_Right:
                    m_marblePresenter->moveByStep(1, 0, mode);
                    break;
                case Qt::Key_Up:
                    m_marblePresenter->moveByStep(0, -1, mode);
                    break;
                case Qt::Key_Down:
                    m_marblePresenter->moveByStep(0, 1, mode);
                    break;
                case Qt::Key_Plus:
                    m_marblePresenter->zoomIn();
                    break;
                case Qt::Key_Minus:
                    m_marblePresenter->zoomOut();
                    break;
                case Qt::Key_Home:
                    m_marblePresenter->goHome();
                    break;
                default:
                    accept = false;
                    break;
            }
            if (accept)
                event->accept();
            return accept;
        }

    private:
        MarbleAbstractPresenter *m_marblePresenter;
    };

    class MarbleQuickItemPrivate : public MarbleAbstractPresenter
    {
    public:
        MarbleQuickItemPrivate(MarbleQuickItem *marble) : MarbleAbstractPresenter()
          ,m_marble(marble)
          ,m_inputHandler(this)
        {
            connect(this, SIGNAL(updateRequired()), m_marble, SLOT(update()));
        }

    private:
        MarbleQuickItem *m_marble;
        friend class MarbleQuickItem;

        MarbleQuickInputHandler m_inputHandler;
    };

    MarbleQuickItem::MarbleQuickItem(QQuickItem *parent) : QQuickPaintedItem(parent)
      ,d(new MarbleQuickItemPrivate(this))
    {
        setSize(QSizeF(800, 800));
        setFocus(true);

        // Initialize the map and forward some signals.
        d->map()->setSize(width(), height());
        d->map()->setShowFrameRate(false);
        d->map()->setProjection(Spherical);
        d->map()->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
        connect(d->map(), SIGNAL(repaintNeeded(QRegion)), this, SLOT(update()));

        setAcceptedMouseButtons(Qt::AllButtons);
        installEventFilter(&d->m_inputHandler);
    }

    void MarbleQuickItem::paint(QPainter *painter)
    {   //Simple copy&port of MarbleWidget paintEvent - much to be done here still (TODO)
        QPaintDevice *paintDevice = painter->device();
        QImage image;
        QRect rect = contentsBoundingRect().toRect();
        if (!isEnabled())
        {
            // If the globe covers fully the screen then we can use the faster
            // RGB32 as there are no translucent areas involved.
            QImage::Format imageFormat = ( d->map()->viewport()->mapCoversViewport() )
                                         ? QImage::Format_RGB32
                                         : QImage::Format_ARGB32_Premultiplied;
            // Paint to an intermediate image
            image = QImage(rect.size(), imageFormat);
            image.fill(Qt::transparent);
            paintDevice = &image;
        }

        {
            painter->end();
            GeoPainter geoPainter(paintDevice, d->map()->viewport(), d->map()->mapQuality());
            geoPainter.setOpacity(0.9);
            d->map()->paint(geoPainter, rect);
        }
        painter->begin(paintDevice);

        if (!isEnabled())
        {
            // Draw a grayscale version of the intermediate image
            QRgb* pixel = reinterpret_cast<QRgb*>(image.scanLine( 0 ));
            for (int i=0; i<image.width()*image.height(); ++i, ++pixel) {
                int gray = qGray(*pixel);
                *pixel = qRgb(gray, gray, gray);
            }

            painter->drawImage(rect, image);
        }
    }

    void MarbleQuickItem::classBegin()
    {
    }

    void MarbleQuickItem::componentComplete()
    {
    }
}
