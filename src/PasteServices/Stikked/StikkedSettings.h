#ifndef STIKKEDSETTINGS_H
#define STIKKEDSETTINGS_H

#include <QHash>
#include "../BasePasteServiceWidget.h"

namespace Ui {
class StikkedSettings;
}

class StikkedSettings : public BasePasteServiceWidget
{
	Q_OBJECT
public:
	explicit StikkedSettings(BasePasteServiceWidget::Mode mode, QWidget *parent = 0);
	~StikkedSettings();
	void load(QSettings *settings);
	void load(BasePasteService *service);
	QHash<QString, QVariant> settings();
	
signals:
	
public slots:

private:
	Ui::StikkedSettings *ui;
	
	int langIndexFromName(QString name);
	int expireIndexFromDuration(int d);
};

#endif // STIKKEDSETTINGS_H
