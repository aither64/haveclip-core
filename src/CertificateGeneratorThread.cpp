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

#include "CertificateGeneratorThread.h"

#include <stdio.h>
#include <QThread>
#include <QDebug>

CertificateGeneratorThread::CertificateGeneratorThread(QString commonName, QObject *parent)
	: QObject(parent),
	  m_done(false),
	  m_commonName(commonName)
{
}

CertificateGeneratorThread::~CertificateGeneratorThread()
{
	X509_free(m_x509);
	EVP_PKEY_free(m_pkey);

#ifndef OPENSSL_NO_ENGINE
	ENGINE_cleanup();
#endif

	CRYPTO_cleanup_all_ex_data();

	CRYPTO_mem_leaks(m_bio);
	BIO_free(m_bio);
}

void CertificateGeneratorThread::generate()
{
	connect(QThread::currentThread(), SIGNAL(finished()), this, SLOT(finish()));

	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

	if ((m_bio = BIO_new_fp(stderr, BIO_NOCLOSE)) == NULL)
		return error();

	RSA *rsa;
	BIGNUM *e;
	X509_NAME *name = NULL;
	int bits = 2048;
	long days = 10 * 365;

	if ((m_pkey = EVP_PKEY_new()) == NULL)
		return error();

	if ((m_x509 = X509_new()) == NULL)
		return error();

	if ((rsa = RSA_new()) == NULL)
		return error();

	if ((e = BN_new()) == NULL)
		return error();

	// Cannot generate a prime - connection does not work when exponent is
	// bigger than 65537.
	if (!BN_set_word(e, 65537))
		return error();

	if (RSA_generate_key_ex(rsa, bits, e, NULL) == NULL)
		return error();

	if (!EVP_PKEY_assign_RSA(m_pkey, rsa))
		return error();

	rsa = NULL;

	X509_set_version(m_x509, 2);
	ASN1_INTEGER_set(X509_get_serialNumber(m_x509), 0);
	X509_gmtime_adj(X509_get_notBefore(m_x509), 0);
	X509_gmtime_adj(X509_get_notAfter(m_x509), 60 * 60 * 24 * days);
	X509_set_pubkey(m_x509, m_pkey);

	name = X509_get_subject_name(m_x509);

	X509_NAME_add_entry_by_txt(
		name, "C", MBSTRING_ASC,
		reinterpret_cast<const unsigned char*>("CZ"),
		-1, -1, 0
	);
	X509_NAME_add_entry_by_txt(
		name, "CN", MBSTRING_ASC,
		reinterpret_cast<const unsigned char*>(m_commonName.toLocal8Bit().constData()),
		-1, -1, 0
	);
	X509_NAME_add_entry_by_txt(
		name, "O", MBSTRING_ASC,
		reinterpret_cast<const unsigned char*>("HaveFun.cz"),
		-1, -1, 0
	);
	X509_NAME_add_entry_by_txt(
		name, "OU", MBSTRING_ASC,
		reinterpret_cast<const unsigned char*>("HaveClip"),
		-1, -1, 0
	);

	X509_set_issuer_name(m_x509, name);

	if (!addExtension(NID_basic_constraints, "critical,CA:FALSE,pathlen:0"))
		return error();

	if (!addExtension(NID_subject_key_identifier, "hash"))
		return error();

	if (!X509_sign(m_x509, m_pkey, EVP_sha1()))
		return error();

	BN_clear_free(e);

	m_done = true;

	QThread::currentThread()->quit();
}

void CertificateGeneratorThread::savePrivateKeyToFile(const QString &path)
{
	FILE *f;

	if ((f = fopen(path.toLocal8Bit().constData(), "w")) == NULL)
		return error();

	PEM_write_PrivateKey(f, m_pkey, NULL, NULL, 0, NULL, NULL);

	fclose(f);

}

void CertificateGeneratorThread::saveCertificateToFile(const QString &path)
{
	FILE *f;

	if ((f = fopen(path.toLocal8Bit().constData(), "w")) == NULL)
		return error();

	PEM_write_X509(f, m_x509);

	fclose(f);
}

void CertificateGeneratorThread::finish()
{
	if (m_done)
		emit finished();
}

bool CertificateGeneratorThread::addExtension(int nid, char *value)
{
	X509_EXTENSION *ex;
	X509V3_CTX ctx;

	X509V3_set_ctx_nodb(&ctx);
	X509V3_set_ctx(&ctx, m_x509, m_x509, NULL, NULL, 0);

	if (!(ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value)))
		return false;

	X509_add_ext(m_x509, ex, -1);
	X509_EXTENSION_free(ex);

	return true;
}

void CertificateGeneratorThread::error()
{
	char str[255];
	unsigned long e = ERR_get_error();
	QString qerr;

	if (e > 0)
	{
		ERR_error_string_n(e, str, sizeof(str));
		qerr = QString(str);
	}

	QThread::currentThread()->quit();

	emit errorOccurred(qerr);
}
