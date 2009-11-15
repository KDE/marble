#include "imagewidget.h"
imageWidget::imageWidget()
{
}

imageWidget::~imageWidget()
{
}

void imageWidget::addImage(QPixmap temp)
{
    image=temp;
}


void imageWidget::paintEvent(QPaintEvent * event )
{
mDebug()<<__func__<<"painevent";
}

void imageWidget::mousePressEvent ( QMouseEvent * event )
{
mDebug()<<__func__<<"mouseevent";
}

QPixmap * imageWidget::returnPointerToImage()
{
return &image;
}
#include "imagewidget.moc"
