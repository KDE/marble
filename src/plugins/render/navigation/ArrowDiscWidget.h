// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef ARROWDISCWIDGET_H
#define ARROWDISCWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>

namespace Marble
{

class MarbleWidget;

class ArrowDiscWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArrowDiscWidget( QWidget *parent = nullptr );
    ~ArrowDiscWidget() override;

    void setMarbleWidget( MarbleWidget *marbleWidget );

Q_SIGNALS:
    void repaintNeeded();

protected:
    void paintEvent( QPaintEvent * ) override;
    void mouseMoveEvent( QMouseEvent *mouseEvent ) override;
    void mousePressEvent( QMouseEvent *mouseEvent ) override;
    void mouseReleaseEvent( QMouseEvent *mouseEvent ) override;
    void leaveEvent( QEvent *event ) override;
    void repaint();

private Q_SLOTS:
    void startPressRepeat();
    void repeatPress();

private:
    static QPixmap pixmap(const QString &id );
    Qt::ArrowType arrowUnderMouse( const QPoint &position ) const;
    QTimer m_initialPressTimer;
    QTimer m_repeatPressTimer;
    Qt::ArrowType m_arrowPressed;
    int m_repetitions;

    MarbleWidget *m_marbleWidget;
    QString m_imagePath;
};

}

#endif
