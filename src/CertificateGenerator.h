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

#ifndef CERTIFICATEGENERATOR_H
#define CERTIFICATEGENERATOR_H

#include <QObject>

class CertificateGeneratorThread;

class CertificateGenerator : public QObject
{
	Q_OBJECT
public:
	enum ErrorType {
		GenerateFailed,
		MkpathFailed,
		SaveFailed
	};

    Q_ENUMS(ErrorType)

	explicit CertificateGenerator(QObject *parent = 0);
	~CertificateGenerator();

    Q_PROPERTY(QString commonName READ commonName WRITE setCommonName NOTIFY commonNameChanged)
    QString commonName();
	void setCommonName(QString name);

    Q_INVOKABLE void generate();
    Q_INVOKABLE void savePrivateKeyToFile(QString path);
    Q_INVOKABLE void saveCertificateToFile(QString path);

signals:
	void start();
	void commonNameChanged(const QString &name);
	void finished();
	void errorOccurred(CertificateGenerator::ErrorType type, const QString &msg);

private slots:
	void error(const QString &error);
	void finish();

private:
	CertificateGeneratorThread *m_generator;
	QString m_commonName;
	ErrorType m_phase;

	bool mkpath(QString &path);

};

#endif // CERTIFICATEGENERATOR_H
