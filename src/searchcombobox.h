#ifndef SEARCHCOMBOBOX_H
#define SEARCHCOMBOBOX_H

#include <QComboBox>

class SearchComboBox : public QComboBox {
	Q_OBJECT
private:	
public:
	SearchComboBox(QWidget*);
public slots:
	void showListView(QString);
};

#endif // SEARCHCOMBOBOX_H
