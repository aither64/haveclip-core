#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "HaveClip.h"

AboutDialog::AboutDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->textLabel->setText(tr(
		"<h1>HaveClip</h1>"
		"<p>Version %1</p>"
		"<p>© 2013 Jakub Skokan &lt;aither@havefun.cz&gt;</p>"
	).arg(VERSION));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
