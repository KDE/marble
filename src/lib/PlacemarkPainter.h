//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//

//
// PlacemarkPainter is responsible for drawing the Placemarks on the map
//

#ifndef MARBLE_PLACEMARKPAINTER_H
#define MARBLE_PLACEMARKPAINTER_H


#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtGui/QColor>

class QFont;
class QItemSelection;
class QPainter;
class QString;

namespace Marble
{

class ViewportParams;
class VisiblePlacemark;

static const qreal s_labelOutlineWidth = 2.5;


class PlacemarkPainter : public QObject
{
    Q_OBJECT

 public:
    explicit PlacemarkPainter(QObject *parent = 0);
    ~PlacemarkPainter();

    void drawPlacemarks( QPainter* painter, 
                         QVector<VisiblePlacemark*> visiblePlacemarks,
                         const QItemSelection &selection, 
                         ViewportParams *viewport );

    // FIXME: To be removed after MapTheme / KML refactoring
    void setDefaultLabelColor( const QColor& color );

 private:
    enum LabelStyle {
        Normal = 0,
        Glow,
        Selected
    };

    void drawLabelText( QPainter &labelPainter, const QString &text, const QFont &labelFont, LabelStyle labelStyle );
    void drawLabelPixmap( VisiblePlacemark *mark, bool isSelected );

    bool testXBug();

 private:
    bool m_useXWorkaround;  // Indicates need for an X windows workaround.

    // FIXME: To be removed after MapTheme / KML refactoring
    QColor m_defaultLabelColor;
};

}

#endif
