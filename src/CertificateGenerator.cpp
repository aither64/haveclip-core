/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

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

#include "CertificateGenerator.h"

#include <QThread>
#include <QFileInfo>
#include <QDir>

#include "CertificateGeneratorThread.h"

CertificateGenerator::CertificateGenerator(QObject *parent) :
		QObject(parent),
		m_generator(0),
		m_phase(GenerateFailed)
{
}

CertificateGenerator::~CertificateGenerator()
{
	if (m_generator)
		m_generator->deleteLater();
}

QString CertificateGenerator::commonName()
{
    return m_commonName;
}

void CertificateGenerator::setCommonName(QString name)
{
    if (m_commonName != name)
    {
        m_commonName = name;
        emit commonNameChanged(m_commonName);
    }
}

void CertificateGenerator::generate()
{
	QThread *thread = new QThread(this);
	m_generator = new CertificateGeneratorThread(m_commonName);
	m_generator->moveToThread(thread);

	connect(this, SIGNAL(start()), m_generator, SLOT(generate()));
	connect(m_generator, SIGNAL(errorOccurred(QString)), this, SLOT(error(QString)));
	connect(m_generator, SIGNAL(finished()), this, SLOT(finish()));

	thread->start();

	emit start();
}

bool CertificateGenerator::mkpath(QString &path)
{
	QFileInfo f(path);
	QDir d;

	if(d.mkpath(f.absolutePath()))
	{
		return true;

	} else {
		emit errorOccurred(MkpathFailed, f.absolutePath());
		return false;
	}
}

void CertificateGenerator::savePrivateKeyToFile(QString path)
{
	if(!mkpath(path))
		return;

	m_generator->savePrivateKeyToFile(path);
}

void CertificateGenerator::saveCertificateToFile(QString path)
{
	if(!mkpath(path))
		return;

	m_generator->saveCertificateToFile(path);
}

void CertificateGenerator::error(const QString &error)
{
	emit errorOccurred(m_phase, error);
}

void CertificateGenerator::finish()
{
	m_phase = SaveFailed;

	emit finished();
}
