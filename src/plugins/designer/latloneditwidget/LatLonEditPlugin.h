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

#include <QDesignerCustomWidgetInterface>
#include <QObject>
#include <marble_export.h>

class QString;
class QWidget;
class QIcon;

class LatLonEditPlugin : public QObject,
                         public QDesignerCustomWidgetInterface
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.LatLonEditPlugin")
	Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
	explicit LatLonEditPlugin(QObject *parent = 0);

	bool isContainer() const override;
	bool isInitialized() const override;
	QIcon icon() const override;
	QString domXml() const override;
	QString group() const override;
	QString includeFile() const override;
	QString name() const override;
	QString toolTip() const override;
	QString whatsThis() const override;
	QWidget *createWidget(QWidget *parent) override;
	void initialize(QDesignerFormEditorInterface *core) override;

private:
	bool m_initialized;
};
#endif
