#ifndef CERTIFICATETRUSTDIALOG_H
#define CERTIFICATETRUSTDIALOG_H

#include <QDialog>
#include <QSslError>

#include "HaveClip.h"

namespace Ui {
class CertificateTrustDialog;
}

class CertificateTrustDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CertificateTrustDialog(HaveClip::Node *node, const QList<QSslError> &errors, QWidget *parent = 0);
	~CertificateTrustDialog();
	bool remember();
	static QString formatDigest(QByteArray raw);

private:
	Ui::CertificateTrustDialog *ui;
};

#endif // CERTIFICATETRUSTDIALOG_H
