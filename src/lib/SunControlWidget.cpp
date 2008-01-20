// (c) 2008 David Roberts

#include "SunControlWidget.h"

SunControlWidget::SunControlWidget(QWidget* parent, SunLocator* sunLocator) : QDialog(parent), m_sunLocator(sunLocator) {
	m_uiWidget.setupUi(this);
	connect(m_uiWidget.showToolButton, SIGNAL(clicked(bool)), this, SLOT(showSunClicked(bool)));
	connect(m_uiWidget.centerToolButton, SIGNAL(clicked(bool)), this, SLOT(centerSunClicked(bool)));
	
	if(m_sunLocator->getShow()) {
		m_uiWidget.showToolButton->setChecked(true);
		m_uiWidget.showToolButton->setText("&Hide");
	}
}

SunControlWidget::~SunControlWidget() {}

void SunControlWidget::showSunClicked(bool checked) {
	if(checked) m_uiWidget.showToolButton->setText("&Hide");
	else m_uiWidget.showToolButton->setText("Sh&ow");
	emit showSun(checked);
}

void SunControlWidget::centerSunClicked(bool checked) {
	emit centerSun();
}

#include "SunControlWidget.moc"
