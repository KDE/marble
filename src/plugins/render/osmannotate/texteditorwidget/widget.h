#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QWidget>

namespace Ui
{
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
public slots:
    void expandTextEdit();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
