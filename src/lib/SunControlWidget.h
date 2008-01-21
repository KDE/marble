// (c) 2008 David Roberts

#ifndef SUNCONTROLWIDGET_H
#define SUNCONTROLWIDGET_H

#include <QDebug>
#include <QtGui/QDialog>
#include <QTimer>

#include "ui_SunControlWidget.h"
#include "SunLocator.h"

#include "marble_export.h"

class MARBLE_EXPORT SunControlWidget : public QDialog {
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
	
	Q_SIGNALS:
	void showSun(bool show);
	
	private:
	void datetimeChanged(QDateTime datetime);
	
	protected:
	Ui::SunControlWidget m_uiWidget;
	SunLocator* m_sunLocator;
	QTimer* m_timer;
};

#endif
