#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->textLabel->setText(tr(
		"<h1>HaveClip</h1>"
		"<p>Version 0.1.0</p>"
		"<p>© 2013 Jakub Skokan &lt;aither@havefun.cz&gt;</p>"
	));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
