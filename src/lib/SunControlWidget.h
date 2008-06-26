//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      David Roberts <dvdr18@gmail.com>
//


#ifndef SUNCONTROLWIDGET_H
#define SUNCONTROLWIDGET_H


#include <QtGui/QDialog>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include "ui_SunControlWidget.h"
#include "SunLocator.h"

#include "marble_export.h"


class MARBLE_EXPORT SunControlWidget : public QDialog
{
    Q_OBJECT
	
 public:
    SunControlWidget(QWidget* parent, SunLocator* sunLocator);
    ~SunControlWidget();

 private Q_SLOTS:
    void showSunClicked(bool checked);
    void nowClicked(bool checked);
    void showSunShadingClicked(int index);
    void centerSunClicked(bool checked);
    void timeChanged(const QTime& time);
    void dateChanged();
    void hourChanged(int hour);
	
    void updateDateTime();
	void speedChanged(int speed);
	
 Q_SIGNALS:
    void showSun(bool show);
	
 private:
    void datetimeChanged(QDateTime datetime);
	
 protected:
    Q_DISABLE_COPY( SunControlWidget )
    Ui::SunControlWidget m_uiWidget;
    SunLocator* m_sunLocator;
};

#endif
