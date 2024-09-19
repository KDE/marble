// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef NAVIGATIONSLIDER_H
#define NAVIGATIONSLIDER_H

#include <QAbstractSlider>
#include <QPixmap>

namespace Marble
{

class NavigationSlider : public QAbstractSlider
{
    Q_OBJECT
public:
    explicit NavigationSlider(QWidget *parent = nullptr);
    ~NavigationSlider() override;

Q_SIGNALS:
    void repaintNeeded();

protected:
    void enterEvent(QEnterEvent *) override;
    void mouseMoveEvent(QMouseEvent *mouseEvent) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void leaveEvent(QEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void repaint();

private:
    static QPixmap pixmap(const QString &id);
    QString m_handleImagePath;
};

}

#endif
