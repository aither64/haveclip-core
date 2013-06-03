#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include<QSettings>

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
	
private:
	Ui::SettingsDialog *ui;

private slots:
	void addNode();
	void editNode();
	void deleteNode();
};

#endif // SETTINGSDIALOG_H
