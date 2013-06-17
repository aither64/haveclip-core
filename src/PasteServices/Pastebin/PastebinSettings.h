#ifndef PASTEBINSETTINGS_H
#define PASTEBINSETTINGS_H

#include <QHash>
#include "../BasePasteServiceWidget.h"

namespace Ui {
class PastebinSettings;
}

class PastebinSettings : public BasePasteServiceWidget
{
	Q_OBJECT
	
public:
	explicit PastebinSettings(BasePasteServiceWidget::Mode mode, QWidget *parent = 0);
	~PastebinSettings();
	void load(QSettings *settings);
	void load(BasePasteService *service);
	QHash<QString, QVariant> settings();
	
private:
	Ui::PastebinSettings *ui;
	QStringList expireCodes;

	QString expireCodeFromIndex(int i);
	int expireIndexFromCode(QString code);

};

#endif // PASTEBINSETTINGS_H
