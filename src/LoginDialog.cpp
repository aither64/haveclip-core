#include "LoginDialog.h"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QString username, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::LoginDialog)
{
	ui->setupUi(this);

	ui->usernameLineEdit->setText(username);
	ui->errorLabel->hide();
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::setError(QString err)
{
	ui->errorLabel->setText(err);
	ui->errorLabel->show();
}

QString LoginDialog::username()
{
	return ui->usernameLineEdit->text();
}

QString LoginDialog::password()
{
	return ui->passwordLineEdit->text();
}
