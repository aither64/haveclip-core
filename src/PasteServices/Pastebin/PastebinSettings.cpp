#include "PastebinSettings.h"
#include "ui_PastebinSettings.h"

#include "Pastebin.h"

PastebinSettings::PastebinSettings(BasePasteServiceWidget::Mode mode, QWidget *parent) :
	BasePasteServiceWidget(mode, parent),
        ui(new Ui::PastebinSettings)
{
	ui->setupUi(this);

	expireCodes << "10M"
		    << "1H"
		    << "1D"
		    << "1W"
		    << "2W"
		    << "1M"
		    << "N";

	for(int i = 0; Pastebin::m_languages[i].name != 0; i++)
		ui->languageComboBox->addItem(Pastebin::m_languages[i].label);
}

PastebinSettings::~PastebinSettings()
{
	delete ui;
}

void PastebinSettings::load(QSettings *settings)
{
	settings->beginGroup("PasteServices/Pastebin");

	// FIXME: defaults
	ui->nameTitleLineEdit->setText(settings->value("Name").toString());
	ui->exposureComboBox->setCurrentIndex(settings->value("Exposure", Pastebin::Unlisted).toInt());
	ui->languageComboBox->setCurrentIndex(Pastebin::langIndexFromName(Pastebin::m_languages, settings->value("Language", "text").toString()));
	ui->expirationComboBox->setCurrentIndex( expireIndexFromCode(settings->value("Expiration", "1H").toString()) );
	ui->loginGroupBox->setChecked(settings->value("Login", false).toBool());
	ui->usernameLineEdit->setText(settings->value("Username").toString());

	settings->endGroup();
}

void PastebinSettings::load(BasePasteService *service)
{
	Pastebin *pastebin = static_cast<Pastebin*>(service);

	ui->nameTitleLineEdit->setText(pastebin->name());
	ui->exposureComboBox->setCurrentIndex(pastebin->exposure());
	ui->languageComboBox->setCurrentIndex(Pastebin::langIndexFromName(Pastebin::m_languages, pastebin->lang()));
	ui->expirationComboBox->setCurrentIndex( expireIndexFromCode(pastebin->expiration()) );
	ui->loginGroupBox->setChecked(pastebin->login());
	ui->usernameLineEdit->setText(pastebin->username());
}

QHash<QString, QVariant> PastebinSettings::settings()
{
	QHash<QString, QVariant> s;

	s["Name"] = ui->nameTitleLineEdit->text();
	s["Exposure"] =  ui->exposureComboBox->currentIndex();
	s["Lang"] = Pastebin::m_languages[ui->languageComboBox->currentIndex()].name;
	s["Expire"] = expireCodeFromIndex(ui->expirationComboBox->currentIndex());
	s["Login"] = ui->loginGroupBox->isChecked();
	s["Username"] = ui->usernameLineEdit->text();

	return s;
}

QString PastebinSettings::expireCodeFromIndex(int i)
{
	return expireCodes[i];
}

int PastebinSettings::expireIndexFromCode(QString code)
{
	int i = expireCodes.indexOf(code);

	if(i == -1)
		return 0;

	return i;
}
