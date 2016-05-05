/*
  HaveClip

  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CERTIFICATEGENERATORTHREAD_H
#define CERTIFICATEGENERATORTHREAD_H

#include <QObject>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
# include <openssl/engine.h>
#endif

class CertificateGeneratorThread : public QObject
{
	Q_OBJECT
public:
	explicit CertificateGeneratorThread(QString commonName, QObject *parent = 0);
	~CertificateGeneratorThread();

signals:
	void errorOccurred(const QString &error);
	void finished();

public slots:
	void generate();
	void savePrivateKeyToFile(const QString &path);
	void saveCertificateToFile(const QString &path);

private slots:
	void finish();

private:
	bool m_done;
	QString m_commonName;
	BIO *m_bio;
	X509 *m_x509;
	EVP_PKEY *m_pkey;

	bool addExtension(int nid, char *value);
	void error();
};

#endif // CERTIFICATEGENERATORTHREAD_H
