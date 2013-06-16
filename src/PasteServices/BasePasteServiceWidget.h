#ifndef BASEPASTESERVICEWIDGET_H
#define BASEPASTESERVICEWIDGET_H

#include <QWidget>
#include "BasePasteService.h"

class BasePasteServiceWidget : public QWidget
{
	Q_OBJECT
public:
	enum Mode {
		Settings,
		Paste
	};

	explicit BasePasteServiceWidget(Mode mode, QWidget *parent = 0);
	virtual void load(QSettings *settings) = 0;
	virtual void load(BasePasteService *service) = 0;
	virtual QHash<QString, QVariant> settings() = 0;
	
signals:
	
public slots:
	
private:
	Mode mode;
};

#endif // BASEPASTESERVICEWIDGET_H
