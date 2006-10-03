#include "placemarkmodel.h"
#include "katlasdirs.h"
#include <QFile>
#include <QTextStream>
#include <QModelIndex>
#include <QIcon>
#include <QDebug>
#include <QTime>

#ifdef KDEBUILD
#include "placemarkmodel.moc"
#endif

PlaceMarkModel::PlaceMarkModel(QObject* obj) : QStandardItemModel(obj){
	QModelIndex parent;
//	parent = index(0, 0, parent);
	insertColumns(0, 8, parent);

        setHeaderData(0, Qt::Horizontal, tr("Place"));
        setHeaderData(1, Qt::Horizontal, tr("State"));
        setHeaderData(2, Qt::Horizontal, tr("Country"));
        setHeaderData(3, Qt::Horizontal, tr("Role"));
        setHeaderData(4, Qt::Horizontal, tr("Lat"));
        setHeaderData(5, Qt::Horizontal, tr("Lon"));
        setHeaderData(6, Qt::Horizontal, tr("Population"));

//	load(KAtlasDirs::path("placemarks/cities.txt"));
}

void PlaceMarkModel::load(QString filename){
	QTime timer;
	timer.start();

	QFile file( filename );
        file.open( QIODevice::ReadOnly );
        QTextStream stream( &file );  // read the data serialized from the file
	stream.setCodec("UTF-8");

	QString rawline;
	QString name, state, country, role, popstring, latstring, lngstring; 
	float population, lat, lng;

	QIcon* city_1_red = new QIcon(KAtlasDirs::path("bitmaps/city_1_red.png"));
	QIcon* city_1_orange = new QIcon(KAtlasDirs::path("bitmaps/city_1_orange.png"));
	QIcon* city_1_yellow = new QIcon(KAtlasDirs::path("bitmaps/city_1_yellow.png"));
	QIcon* city_1_white = new QIcon(KAtlasDirs::path("bitmaps/city_1_white.png"));

	QIcon* city_2_red = new QIcon(KAtlasDirs::path("bitmaps/city_2_red.png"));
	QIcon* city_2_orange = new QIcon(KAtlasDirs::path("bitmaps/city_2_orange.png"));
	QIcon* city_2_yellow = new QIcon(KAtlasDirs::path("bitmaps/city_2_yellow.png"));
	QIcon* city_2_white = new QIcon(KAtlasDirs::path("bitmaps/city_2_white.png"));

	QIcon* city_3_red = new QIcon(KAtlasDirs::path("bitmaps/city_3_red.png"));
	QIcon* city_3_orange = new QIcon(KAtlasDirs::path("bitmaps/city_3_orange.png"));
	QIcon* city_3_yellow = new QIcon(KAtlasDirs::path("bitmaps/city_3_yellow.png"));
	QIcon* city_3_white = new QIcon(KAtlasDirs::path("bitmaps/city_3_white.png"));

	QIcon* city_4_red = new QIcon(KAtlasDirs::path("bitmaps/city_4_red.png"));
	QIcon* city_4_orange = new QIcon(KAtlasDirs::path("bitmaps/city_4_orange.png"));
	QIcon* city_4_yellow = new QIcon(KAtlasDirs::path("bitmaps/city_4_yellow.png"));
	QIcon* city_4_white = new QIcon(KAtlasDirs::path("bitmaps/city_4_white.png"));

	QIcon* placeicon = 0;

	QStringList splitline;
	int row = 0;

	while(!stream.atEnd()){

		rawline=stream.readLine();
		splitline = rawline.split("\t");

		name = splitline[0];

//		state = splitline[1];
//		country = splitline[2];
//		role = splitline[3];
		popstring = splitline[4];
		latstring = splitline[5];
		lngstring = splitline[6];

		population = 1000*popstring.toFloat();

		placeicon=city_1_red;
		if(population < 10000000) placeicon=city_1_red;


		if(population < 2500) placeicon=city_4_white;
		else if(population < 5000) placeicon=city_4_yellow;
		else if(population < 7500) placeicon=city_4_orange;
		else if(population < 10000) placeicon=city_4_red;
		else if(population < 25000) placeicon=city_3_white;
		else if(population < 50000) placeicon=city_3_yellow;
		else if(population < 75000) placeicon=city_3_orange;
		else if(population < 100000) placeicon=city_3_red;
		else if(population < 250000) placeicon=city_2_white;
		else if(population < 500000) placeicon=city_2_yellow;
		else if(population < 750000) placeicon=city_2_orange;
		else if(population < 1000000) placeicon=city_2_red;
		else if(population < 2500000) placeicon=city_1_white;
		else if(population < 5000000) placeicon=city_1_yellow;
		else if(population < 7500000) placeicon=city_1_orange;

		lng = lngstring.left(lngstring.size()-2).toFloat();
		if (lngstring.contains("E")) lng=-lng;

		lat = latstring.left(latstring.size()-2).toFloat();
		if (latstring.contains("S")) lat=-lat;

		insertRow(row);
/*
                QStandardItem *item = new QStandardItem();
		item->setData(name, Qt::EditRole);
		item->setData(*placeicon, Qt::DecorationRole );
                setItem(row, 0, item);
                item = new QStandardItem();
		item->setData(lat);
                setItem(row, 4, item);
                item = new QStandardItem();
		item->setData(lng);
                setItem(row, 5, item);
*/

		setData(index(row, 0), name, Qt::EditRole);
		setData(index(row, 0), *placeicon, Qt::DecorationRole );

//		setData(index(row, 1, QModelIndex()), state);
//		setData(index(row, 2, QModelIndex()), country);
//		setData(index(row, 3, QModelIndex()), role);
		setData(index(row, 4), lat);
		setData(index(row, 5), lng);
//		setData(index(row, 6, QModelIndex()), population);

		row++;
//		PlaceMark *city = new PlaceMark(name,lat,lng);
//		append( city );

	}

	file.close();
	qDebug() << "Elapsed:" << QString::number( timer.elapsed() );
}
