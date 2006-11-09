#include "placemarkmodel.h"

#include <cmath>
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

PlaceMarkModel::PlaceMarkModel(QObject *parent) : QAbstractListModel(parent){
	m_citysymbol 
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_4_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_3_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_2_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_white.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_yellow.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_orange.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/city_1_red.png"))

	 << QPixmap(KAtlasDirs::path("bitmaps/pole_1.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/pole_2.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/mountain_1.png"))
	 << QPixmap(KAtlasDirs::path("bitmaps/volcano_1.png"));
}

PlaceMarkModel::~PlaceMarkModel(){
}

int PlaceMarkModel::rowCount(const QModelIndex &parent) const {
	return m_placemarkindex.size();
}

int PlaceMarkModel::columnCount(const QModelIndex &parent) const {
	return 6;
}

QVariant PlaceMarkModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
	return QVariant();

	if (index.row() >= m_placemarkindex.size())
		return QVariant();

	if (role == Qt::DisplayRole){
		float lng, lat;
		switch ( index.column() ){
		case 4:
			m_placemarkindex.at(index.row())->coordinate( lng, lat );
			return -lat*180.0/M_PI;
			break;
		case 5:
			m_placemarkindex.at(index.row())->coordinate( lng, lat );
			return -lng*180.0/M_PI;
			break;
		default:
			return m_placemarkindex.at(index.row())->name();
			break;
		}
	}
	if (role == Qt::DecorationRole){
		return m_citysymbol.at(m_placemarkindex.at(index.row())->symbol());
	}
	else
		return QVariant();
}

QVariant PlaceMarkModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

void PlaceMarkModel::setContainer(PlaceContainer* container){

	PlaceContainer::const_iterator it;

	for ( it=container->constBegin(); it != container->constEnd(); it++ ){ // STL iterators
		m_placemarkindex << *it;
	}
	qStableSort( m_placemarkindex.begin(), m_placemarkindex.end(), nameSort );
}
