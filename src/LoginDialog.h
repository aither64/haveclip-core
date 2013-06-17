#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit LoginDialog(QString username = QString(), QWidget *parent = 0);
	~LoginDialog();
	void setError(QString err);
	QString username();
	QString password();
	
private:
	Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
