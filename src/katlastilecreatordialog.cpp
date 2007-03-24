#include "katlastilecreatordialog.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include "tilescissor.h"

KAtlasTileCreatorDialog::KAtlasTileCreatorDialog(QWidget *parent) : QDialog(parent) {

	setupUi(this);

}

void KAtlasTileCreatorDialog::setProgress( int progress ){ 
	progressBar->setValue( progress );
	if ( progress == 100 ) accept(); 
}

void KAtlasTileCreatorDialog::setSummary( const QString& name, const QString& description ){ 
	QString summary = "<B>" + name + "</B><BR>" + description; 
	descriptionLabel->setText( summary );
}

#ifndef Q_OS_MACX
#include "katlastilecreatordialog.moc"
#endif
