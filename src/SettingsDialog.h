#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>
#include <QHash>

#include "HaveClip.h"
#include "PasteServices/BasePasteService.h"

#define NODE_ADD_STR tr("IP address:port")

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit SettingsDialog(QSettings *settings, QWidget *parent = 0);
	~SettingsDialog();
	QStringList nodes();
	bool historyEnabled();
	int historySize();
	QString host();
	int port();
	QString password();
	HaveClip::Encryption encryption();
	QString certificate();
	QString privateKey();
	bool pasteServiceEnabled();
	BasePasteService::PasteService pasteServiceType();
	QHash<QString, QVariant> pasteServiceSettings();
	
private:
	Ui::SettingsDialog *ui;

private slots:
	void addNode();
	void editNode();
	void deleteNode();
	void nodeChange(QString str);
	void setCertificatePath();
	void setPrivateKeyPath();
	void setFingerprint();
	void pasteServiceToggle(bool enable);
};

#endif // SETTINGSDIALOG_H
