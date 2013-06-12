#include <QDebug>
#include <QFileDialog>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QSettings *settings, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->nodeAddButton, SIGNAL(clicked()), this, SLOT(addNode()));
	connect(ui->nodeEditButton, SIGNAL(clicked()), this, SLOT(editNode()));
	connect(ui->nodeRemoveButton, SIGNAL(clicked()), this, SLOT(deleteNode()));
	connect(ui->nodeListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(nodeChange(QString)));

	// Pool
	foreach(QString n, settings->value("Pool/Nodes").toStringList())
	{
		QListWidgetItem *it = new QListWidgetItem(n);
		it->setFlags(it->flags() | Qt::ItemIsEditable);
		ui->nodeListWidget->addItem(it);
	}

	// History
	ui->historyGroupBox->setChecked( settings->value("History/Enable", true).toBool() );
	ui->historySizeSpinBox->setValue( settings->value("History/Size", 10).toInt() );

	// Encryption
	ui->encryptionComboBox->setCurrentIndex(settings->value("Connection/Encryption", HaveClip::None).toInt());
	ui->certificateLineEdit->setText(settings->value("Connection/Certificate", "certs/haveclip.crt").toString());
	ui->keyLineEdit->setText(settings->value("Connection/PrivateKey", "certs/haveclip.key").toString());

	connect(ui->certificateButton, SIGNAL(clicked()), this, SLOT(setCertificatePath()));
	connect(ui->keyButton, SIGNAL(clicked()), this, SLOT(setPrivateKeyPath()));

	// Connection
	ui->hostLineEdit->setText( settings->value("Connection/Host", "0.0.0.0").toString() );
	ui->portSpinBox->setValue( settings->value("Connection/Port", 9999).toInt() );

	ui->passwordLineEdit->setText( settings->value("AccessPolicy/Password").toString() );
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

QStringList SettingsDialog::nodes()
{
	QStringList ret;

	for(int i = 0; i < ui->nodeListWidget->count(); i++)
		ret << ui->nodeListWidget->item(i)->text();

	return ret;
}

bool SettingsDialog::historyEnabled()
{
	return ui->historyGroupBox->isChecked();
}

int SettingsDialog::historySize()
{
	return ui->historySizeSpinBox->value();
}

void SettingsDialog::addNode()
{
	QListWidgetItem *it = new QListWidgetItem(NODE_ADD_STR);
	it->setFlags(it->flags() | Qt::ItemIsEditable);
	ui->nodeListWidget->addItem(it);
	ui->nodeListWidget->editItem(it);
}

void SettingsDialog::editNode()
{
	QList<QListWidgetItem*> items = ui->nodeListWidget->selectedItems();

	if(items.count() > 0)
		ui->nodeListWidget->editItem(items.first());
}

void SettingsDialog::deleteNode()
{
	foreach(QListWidgetItem *it, ui->nodeListWidget->selectedItems())
		delete ui->nodeListWidget->takeItem( ui->nodeListWidget->row(it) );
}

void SettingsDialog::nodeChange(QString str)
{
	int cnt = ui->nodeListWidget->count();

	for(int i = 0; i < cnt; i++)
	{
		QListWidgetItem *it = ui->nodeListWidget->item(i);

		if(it->text().isEmpty() || it->text() == NODE_ADD_STR)
			delete ui->nodeListWidget->takeItem( ui->nodeListWidget->row(it) );
	}
}

QString SettingsDialog::host()
{
	return ui->hostLineEdit->text();
}

int SettingsDialog::port()
{
	return ui->portSpinBox->value();
}

QString SettingsDialog::password()
{
	return ui->passwordLineEdit->text();
}

HaveClip::Encryption SettingsDialog::encryption()
{
	return (HaveClip::Encryption) ui->encryptionComboBox->currentIndex();
}

void SettingsDialog::setCertificatePath()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Select certificate file"), "", tr("Certificates (*.crt *.pem)"));

	if(!path.isEmpty())
		ui->certificateLineEdit->setText(path);
}

void SettingsDialog::setPrivateKeyPath()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Select private key file"), "", tr("Private keys (*.key *.pem)"));

	if(!path.isEmpty())
		ui->keyLineEdit->setText(path);
}

QString SettingsDialog::certificate()
{
	return ui->certificateLineEdit->text();
}

QString SettingsDialog::privateKey()
{
	return ui->keyLineEdit->text();
}
