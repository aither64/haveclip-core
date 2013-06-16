#ifndef PASTEDIALOG_H
#define PASTEDIALOG_H

#include <QDialog>
#include "BasePasteService.h"
#include "ui_StikkedSettings.h"
#include "BasePasteServiceWidget.h"

namespace Ui {
class PasteDialog;
}

class PasteDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit PasteDialog(QString data, BasePasteService *service, QWidget *parent = 0);
	~PasteDialog();
	QHash<QString, QVariant> pasteServiceSettings();
	QString dataToPaste();
	
private:
	Ui::PasteDialog *ui;
	BasePasteService *service;
	BasePasteServiceWidget *serviceWidget;
};

#endif // PASTEDIALOG_H
