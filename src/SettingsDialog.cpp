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

	foreach(QString n, settings->value("Pool/Nodes").toStringList())
	{
		QListWidgetItem *it = new QListWidgetItem(n);
		it->setFlags(it->flags() | Qt::ItemIsEditable);
		ui->nodeListWidget->addItem(it);
	}
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

void SettingsDialog::addNode()
{
	QListWidgetItem *it = new QListWidgetItem(ui->nodeLineEdit->text());
	it->setFlags(it->flags() | Qt::ItemIsEditable);
	ui->nodeListWidget->addItem(it);
	ui->nodeLineEdit->clear();
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
	{
		delete ui->nodeListWidget->takeItem( ui->nodeListWidget->row(it) );
	}
}
