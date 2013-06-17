#include "PasteDialog.h"
#include "ui_PasteDialog.h"

#include "Stikked/Stikked.h"
#include "Stikked/StikkedSettings.h"
#include "Pastebin/Pastebin.h"
#include "Pastebin/PastebinSettings.h"

PasteDialog::PasteDialog(QString data, BasePasteService *service, QWidget *parent) :
        QDialog(parent),
	ui(new Ui::PasteDialog),
	service(service)
{
	ui->setupUi(this);

	switch(service->type())
	{
	case BasePasteService::Stikked:
		serviceWidget = new StikkedSettings(BasePasteServiceWidget::Paste, this);
		serviceWidget->load(service);
		break;
	case BasePasteService::Pastebin:
		serviceWidget = new PastebinSettings(BasePasteServiceWidget::Paste, this);
		serviceWidget->load(service);
		break;
	default:
		return;
	}

	ui->pasteOptionsVerticalLayout->addWidget(serviceWidget);

	ui->plainTextEdit->setPlainText(data);
}

PasteDialog::~PasteDialog()
{
	delete ui;
}

QString PasteDialog::dataToPaste()
{
	return ui->plainTextEdit->toPlainText();
}

QHash<QString, QVariant> PasteDialog::pasteServiceSettings()
{
	return serviceWidget->settings();
}
