#include "placemarkinfodialog.h"
#include "placemark.h"
#include <QLabel>

#include <QFile>
#include <QPainter>
#include <QSvgRenderer>

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

	flag_val_lbl->setPixmap( flag( m_mark->countryCode() ) );
	QString description = m_mark->description();
	if ( !description.isEmpty() )
		description_val_browser->setPlainText( description );

	float lng, lat;
	m_mark->coordinate( lng, lat );

	coordinates_val_lbl->setText( GeoPoint( lng, lat ).toString() );

	country_val_lbl->setText( m_mark->countryCode() );

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

const QPixmap PlaceMarkInfoDialog::flag( const QString& countrycode ){
	QPixmap pixmap( flag_val_lbl->size() );

	QString filename = KAtlasDirs::path( QString("flags/flag_%1.svg").arg( countrycode.toLower() ) );

	if ( QFile::exists( filename ) ){
		QSvgRenderer svgobj( filename, this );

		QPainter painter( &pixmap );
		painter.setRenderHint(QPainter::Antialiasing, true);
		QRect viewport( pixmap.rect() );

		painter.setViewport( viewport );
		svgobj.render(&painter);
		return pixmap;
	}
	else return QPixmap();
} 
