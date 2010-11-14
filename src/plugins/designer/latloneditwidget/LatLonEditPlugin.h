/*                                                                                                             
// This file is part of the Marble Virtual Globe.                                                              
//                                                                                                             
// This program is free software licensed under the GNU LGPL. You can                                          
// find a copy of this license in LICENSE.txt in the top directory of                                          
// the source code.                                                                                            
*/
//Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef LATLONEDITPLUGIN_H
#define LATLONEDITPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class QObject;
class QString;
class QWidget;
class QIcon;

class LatLonEditPlugin : public QObject,
                         public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
	explicit LatLonEditPlugin(QObject *parent = 0);

	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget *createWidget(QWidget *parent);
	void initialize(QDesignerFormEditorInterface *core);

private:
	bool m_initialized;
};
#endif
