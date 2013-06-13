#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>

#include "HaveClip.h"

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
};

#endif // SETTINGSDIALOG_H
