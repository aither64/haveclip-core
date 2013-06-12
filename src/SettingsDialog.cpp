#include <QDebug>

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
