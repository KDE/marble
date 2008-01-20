// (c) 2008 David Roberts

#include <QtGui/QComboBox>

#include "SunControlWidget.h"

SunControlWidget::SunControlWidget(QWidget* parent, SunLocator* sunLocator) : QDialog(parent), m_sunLocator(sunLocator) {
	m_uiWidget.setupUi(this);
	connect(m_uiWidget.showToolButton, SIGNAL(clicked(bool)), this, SLOT(showSunClicked(bool)));
	connect(m_uiWidget.centerToolButton, SIGNAL(clicked(bool)), this, SLOT(centerSunClicked(bool)));
    connect(m_uiWidget.sunShadingComboBox, SIGNAL( currentIndexChanged ( int ) ), this, SLOT(showSunShadingClicked( int )));
	
    setModal( false );

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

void SunControlWidget::showSunShadingClicked( int index ) {
    if ( index == 0 )
        emit showSunShading(false);
    if ( index == 1 )
        emit showSunShading(true);
}

void SunControlWidget::centerSunClicked(bool checked) {
	emit centerSun();
}

#include "SunControlWidget.moc"
