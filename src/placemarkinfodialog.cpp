#include "placemarkinfodialog.h"
#include "placemark.h"
#include <QLabel>
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
			rolestring = "Mountain";
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

	QString description = m_mark->description();
	if ( !description.isEmpty() )
		description_val_browser->setPlainText( description );

	float lng, lat;
	m_mark->coordinate( lat, lng );

	QString nsstring = ( lng < 0 ) ? "N" : "S";  
	QString westring = ( lat < 0 ) ? "W" : "E";  

	lng = fabs( lng * 360.0 / (2.0 * M_PI) );

	int lngdeg = (int)lng;
	int lngmin = (int) ( 60 * (lng - lngdeg) );
	int lngsec = (int) ( 3600 * (lng - lngdeg - ((float)(lngmin) / 60) ) );

	lat = fabs( lat * 360.0 / (2.0 * M_PI) );

	int latdeg = (int)lat;
	int latmin = (int) ( 60 * (lat - latdeg) );
	int latsec = (int) ( 3600 * (lat - latdeg - ((float)(latmin) / 60) ) );

	QString coordstring = QString("%1\xb0 %2\' %3\" %4\n%5\xb0 %6\' %7\" %8").arg(lngdeg).arg(lngmin).arg(lngsec).arg(nsstring).arg(latdeg).arg(latmin).arg(latsec).arg(westring);
	coordinates_val_lbl->setText( coordstring );

	country_val_lbl->setText( "-" );

	if ( m_mark->role() == 'H' || m_mark->role() == 'V'){
		population_val_lbl->setVisible( false );
		population_lbl->setVisible( false );

		elevation_val_lbl->setText( QString("%1 m").arg( m_mark->population() / 1000 ) );
	}
	else if (m_mark->role() == 'P' || m_mark->role() == 'M'){
		population_val_lbl->setVisible( false );
		population_lbl->setVisible( false );
		elevation_val_lbl->setVisible( false );
		elevation_lbl->setVisible( false );
	}
	else{
		population_val_lbl->setText( QString::number( m_mark->population() ) );
		elevation_val_lbl->setText( "-" );
	}
}
