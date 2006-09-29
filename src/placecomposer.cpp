#include "placecomposer.h"

#include <QStringList>
#include <QStringListModel>
#include "placelist.h"

#include <QDebug>

PlaceComposer::PlaceComposer(){
	cities = new PlaceList();
}

void PlaceComposer::getPlaceMarks(QStringListModel* placemarkmodel){

//	QModelIndex qmi;
	QStringList placemarklist = cities->getPlaceMarkList();
	placemarkmodel->setStringList(placemarklist);

	for (int row=0; row < placemarklist.size();row++){

		QModelIndex qmi = placemarkmodel->index(row,0,QModelIndex());
		placemarkmodel->setData(qmi, QIcon("/home/tackat/globepedia/src/icons/gohome.png") ,Qt::DecorationRole); // Qt::PlaceMarkType
//		placemarkmodel->setData(qmi, QVariant(i) ,32); // Qt::ObjectID

//		placemarkmodel->setData(qmi, QString("test") ,Qt::EditRole); // Qt::ObjectID

//		placemarkmodel->setData(qmi, (placemarklist[i].toLatin1()).toLower() ,33);
//		placemarkmodel->setData(qmi, QVariant(QString("city")) ,34); // Qt::PlaceMarkType
//		QString text = placemarkmodel->data(qmi, Qt::DisplayRole).toString();
		QString text = placemarkmodel->data(qmi, Qt::DisplayRole).toString();
//		qDebug() << row << text << (placemarkmodel->data(qmi, Qt::UserRole)).toString();

	}

	placemarkmodel->sort(0);

//	QStringListModel::iterator itPlaceMark;
//	const QVector<PlaceMark*>::iterator itEndPlaceMark = end();

//	for ( itPlaceMark = begin(); itPlaceMark != itEndPlaceMark; ++itPlaceMark ){
//		placemarklist << (*itPlaceMark)->getName();
//	}

		
}
