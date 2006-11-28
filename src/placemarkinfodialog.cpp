#include "placemarkinfodialog.h"
#include "placemark.h"
#include <QLabel>

#include <QFile>
#include <QLocale>
#include <QPainter>
#include <QTimer>

#include "katlasdirs.h"
#include <cmath>

#ifdef KDEBUILD
#include "placemarkinfodialog.moc"
#endif


PlaceMarkInfoDialog::PlaceMarkInfoDialog(PlaceMark* mark, QWidget *parent) : QDialog(parent), m_mark(mark) {

	setupUi(this);
	showContent();
}

void PlaceMarkInfoDialog::showContent(){

	name_val_lbl->setText( "<H1><b>" + m_mark->name() + "</b></H1>" );
	altername_val_lbl->setText( "" );

	QString rolestring;
	switch ( m_mark->role().toLatin1() ){
		case 'C':
			rolestring = "Capital";
			break;
		case 'B':
			rolestring = "Capital";
			break;
		case 'R':
			rolestring = "Regional Capital";
			break;
		case 'P':
			rolestring = "Location";
			break;
		case 'M':
			rolestring = "Location";
			break;
		case 'H':
			if ( m_mark->population() > 0)
				rolestring = "Mountain";
			else rolestring = "Elevation extreme";
			break;
		case 'V':
			rolestring = "Volcano";
			break;
		default:
		case 'N':
			rolestring = "City";
			break;
	}

	role_val_lbl->setText( rolestring );

	m_flagcreator = new KAtlasFlag( this );
	requestFlag( m_mark->countryCode() );

	QString description = m_mark->description();
	if ( !description.isEmpty() )
		description_val_browser->setPlainText( description );

	coordinates_val_lbl->setText( m_mark->coordinate().toString() );

	country_val_lbl->setText( m_mark->countryCode() );

	if ( m_mark->role() == 'H' || m_mark->role() == 'V'){
		population_val_lbl->setVisible( false );
		population_lbl->setVisible( false );

		elevation_val_lbl->setText( QString("%1 m").arg( QLocale::system().toString( m_mark->population() / 1000 ) ) );
	}
	else if (m_mark->role() == 'P' || m_mark->role() == 'M'){
		population_val_lbl->setVisible( false );
		population_lbl->setVisible( false );
		elevation_val_lbl->setVisible( false );
		elevation_lbl->setVisible( false );
	}
	else{
		population_val_lbl->setText( QString("%1 inh.").arg(QLocale::system().toString( m_mark->population() ) ) );
		elevation_val_lbl->setText( "-" );
	}
}

void PlaceMarkInfoDialog::requestFlag( const QString& countrycode ){

	QString filename = KAtlasDirs::path( QString("flags/flag_%1.svg").arg( countrycode.toLower() ) );
	m_flagcreator->setFlag( filename, flag_val_lbl->size() );

	if ( QFile::exists( filename ) ){
		connect( m_flagcreator, SIGNAL( flagDone() ), this, SLOT( setFlagLabel() ) );
		QTimer::singleShot(100, m_flagcreator, SLOT(slotDrawFlag()));	
	}
} 

void PlaceMarkInfoDialog::setFlagLabel(){
	flag_val_lbl->setPixmap( m_flagcreator->flag() );
}
