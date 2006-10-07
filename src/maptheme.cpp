#include "maptheme.h"
#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QIcon>
#include <QStringList>
#include <QStandardItemModel>

#include "katlasdirs.h"
#ifdef KDEBUILD
#include "maptheme.moc"
#endif

MapTheme::MapTheme(QObject *parent)
    : QObject(parent)
{
}

int MapTheme::open( QString path ){
	QDomDocument doc( "DeskGlobeML" );
	QFile file( path );
	if( !file.open( QIODevice::ReadOnly ) ){
		qDebug() << QString("Could not open ") + path;
		return -1;
	}
	if( !doc.setContent( &file ) ){
		qDebug() << QString("Parse error!");
		file.close();
		return -2;
	}
	file.close();
	QDomElement element = doc.documentElement();
	if( element.tagName() != "DeskGlobeML" ){
		qDebug("File is not a DeskGlobe file");
		return -3;
	}

	element = element.firstChildElement();

	while( !element.isNull() ){

		if( element.tagName().toLower() == "mapstyle" ){
//			qDebug("Parsing MapStyleNode");

			QDomElement mapStyleSibling = element.firstChildElement();
			while( !mapStyleSibling.isNull() ){

				if( mapStyleSibling.tagName().toLower() == "name" ){
					m_name=mapStyleSibling.text();
//					qDebug() << m_name;
				}

				if( mapStyleSibling.tagName().toLower() == "prefix" ){
					m_prefix=mapStyleSibling.text();
//					qDebug() << m_prefix;
				}

				if( mapStyleSibling.tagName().toLower() == "icon" ){
					m_icon=mapStyleSibling.text();
//					qDebug() << m_icon;
				}
				
				if( mapStyleSibling.tagName().toLower() == "description" ){
					m_description=mapStyleSibling.text();
//					qDebug() << m_description;
				}

				if( mapStyleSibling.tagName().toLower() == "installmap" ){
					m_installmap=mapStyleSibling.text();
//					qDebug() << m_installmap;
				}

				m_labelcolor = QColor( 0, 0, 0, 255 );
				if( mapStyleSibling.tagName().toLower() == "labelstyle" ){
					QDomElement labelStyleSibling = mapStyleSibling.firstChildElement();
					while( !labelStyleSibling.isNull() ){
						if( labelStyleSibling.tagName().toLower() == "color" ){
							m_labelcolor.setNamedColor( "#" + labelStyleSibling.text() );
//							qDebug() << "#" + labelStyleSibling.text();
						}
						labelStyleSibling = labelStyleSibling.nextSiblingElement();
					}
				}

				m_labelcolor.setAlpha(255);

				if( mapStyleSibling.tagName().toLower() == "layer" ){

					m_bitmaplayer.enabled = false;
					if ( mapStyleSibling.attribute( "type", "" ) == "bitmap" ){
						m_bitmaplayer.enabled = true;
						m_bitmaplayer.name = mapStyleSibling.attribute( "name", "" );
						m_bitmaplayer.type = mapStyleSibling.attribute( "type", "" );
						m_bitmaplayer.dem = mapStyleSibling.attribute( "dem", "" );
						if ( m_bitmaplayer.dem == "" ) m_bitmaplayer.dem = "false";
					}

					m_vectorlayer.enabled = false;
					if ( mapStyleSibling.attribute( "type", "" ) == "vector" ){
						m_vectorlayer.enabled = true;
						m_vectorlayer.name = mapStyleSibling.attribute( "name", "" );
						m_vectorlayer.type = mapStyleSibling.attribute( "type", "" );
					}					
//					qDebug() << m_layer.name << " " << m_layer.type;
				}

				mapStyleSibling = mapStyleSibling.nextSiblingElement();

			}

		}

		element = element.nextSiblingElement();
	}

	detectMaxTileLevel();

	return 0;
}

void MapTheme::detectMaxTileLevel(){

	bool noerr = true; 

	int tilelevel = -1;
	int trylevel = 1;

	if ( m_bitmaplayer.type.toLower() == "bitmap" ){
		while ( noerr == true ){
			for ( int n=0; n < 2*trylevel; n++) {
				for ( int m=0; m < trylevel; m++){
					QString tilepath = KAtlasDirs::path( "maps/" + tilePrefix()
						+ QString("_%1_%2x%3.jpg").arg(trylevel).arg(n).arg(m) );
//					qDebug() << tilepath;
					noerr = QFile::exists( tilepath );
					if ( noerr == false ) break; 
				}
				if ( noerr == false ) break; 
			}	

			if ( noerr == true) tilelevel = trylevel;
			trylevel *= 2;
		}

		if ( tilelevel == -1 ){
			qDebug("No Tiles Found!");
		}
	}

	m_maxtilelevel = tilelevel;

}

QStringList MapTheme::findMapThemes( const QString& path ){

	QDir localdirs = QDir( KAtlasDirs::localDir() + "/" + path );
	QDir sysdirs = QDir( KAtlasDirs::systemDir() + "/" + path );

	QStringList localmappaths = localdirs.entryList( QStringList( "*" ),
                                    QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QStringList sysmappaths = sysdirs.entryList( QStringList( "*" ),
                                    QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QStringList localmapdirs;
	QStringList sysmapdirs;

	for (int i = 0; i < localmappaths.size(); ++i){
//		qDebug() << "Map dirs: "  << KAtlasDirs::localDir() + "/maps/" + localmappaths.at(i);
		localmapdirs << KAtlasDirs::localDir() + "/maps/" + localmappaths.at(i);
	}

	for (int i = 0; i < sysmappaths.size(); ++i){
//		qDebug() << "Map dirs: " << KAtlasDirs::systemDir() + "/maps/" + sysmappaths.at(i);
		sysmapdirs << KAtlasDirs::systemDir() + "/maps/" + sysmappaths.at(i);
	}

        QStringList mapfiles;
	QStringList tmp;
	QString themedir, themedirname;
	QString themexml;

	QStringListIterator i( localmapdirs );
	while (i.hasNext()){
		themedir = i.next() + "/";
		themedirname = QDir( themedir ).dirName();
//		qDebug() << themedir;

		tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                    QDir::Files | QDir::NoSymLinks );
		if (!tmp.isEmpty()){
			QStringListIterator k( tmp );
			while (k.hasNext()){
				themexml = k.next();
//				qDebug() << themedirname + "/" + themexml;
				mapfiles << themedirname + "/" + themexml;
			}
		}
//		else qDebug("Empty *.dgml list!");
	}

	QStringListIterator j( sysmapdirs );
	while (j.hasNext()){
		themedir = j.next();
//		qDebug() << themedir;
		themedirname = QDir( themedir ).dirName();

		tmp = ( QDir( themedir ) ).entryList( QStringList( "*.dgml" ),
                                    QDir::Files | QDir::NoSymLinks );
		if (!tmp.isEmpty()){
			QStringListIterator l( tmp );
			while (l.hasNext()){
				themexml = l.next();
//				qDebug() << themedirname + "/" + themexml;
				mapfiles << themedirname + "/" + themexml;
			}
		}
//		else qDebug("Empty *.dgml list!");
	}

	mapfiles.sort();

	for (int i = 1; i < mapfiles.size(); ++i){
		if ( mapfiles.at(i) == mapfiles.at(i-1) ){
			mapfiles.removeAt(i);
			--i;
		}
	}

//	for (int i = 0; i < mapfiles.size(); ++i)
//		qDebug() << "Files: " << mapfiles.at(i);

	return mapfiles;
}

QStandardItemModel* MapTheme::mapThemeModel( const QStringList& stringlist ){

	QStandardItemModel* mapthememodel = new QStandardItemModel();

	QModelIndex parent;
	mapthememodel->insertColumns(0, 3, parent);

        mapthememodel->setHeaderData(0, Qt::Horizontal, tr("Name"));
        mapthememodel->setHeaderData(1, Qt::Horizontal, tr("Description"));
        mapthememodel->setHeaderData(2, Qt::Horizontal, tr("Path"));

	QStringListIterator it(stringlist);

	MapTheme* maptheme = new MapTheme();

	int row = 0;
	while (it.hasNext()){
		QString currentmaptheme = "maps/" + it.next();
		maptheme->open( KAtlasDirs::path( currentmaptheme ) );

		mapthememodel->insertRows(row, 1, QModelIndex());
		mapthememodel->setData( mapthememodel->index(row, 0, QModelIndex()), maptheme->name(), Qt::DisplayRole );
		mapthememodel->setData( mapthememodel->index(row, 0, QModelIndex()), QIcon( KAtlasDirs::path( "maps/" +  maptheme->prefix() + "/" + maptheme->icon() ) ), Qt::DecorationRole );
		mapthememodel->setData( mapthememodel->index(row, 1, QModelIndex()), maptheme->description(), Qt::ToolTipRole);
		mapthememodel->setData( mapthememodel->index(row, 2, QModelIndex()), currentmaptheme );
	}

	return mapthememodel;
}
