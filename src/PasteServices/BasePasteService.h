/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

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

#ifndef BASEPASTESERVICE_H
#define BASEPASTESERVICE_H

#include <QObject>
#include <QSettings>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class BasePasteService : public QObject
{
	Q_OBJECT
public:
	struct Language {
		const char *name;
		const char *label;
	};

	enum PasteService {
		Stikked=0,
		Pastebin,
		PasteServiceCount,
		None
	};

	explicit BasePasteService(QSettings *settings, QObject *parent = 0);
	virtual PasteService type() = 0;
	virtual QString internalName() = 0;
	virtual QString label() = 0;
	virtual void applySettings(QHash<QString, QVariant> s) = 0;
	static int langIndexFromName(Language *lang, QString name);

signals:
	void authenticationRequired(QString username, bool failed, QString msg);
	void pasteFailed(QString error);
	void pasted(QUrl url);
	void errorOccured(QString error);
	
public slots:
	virtual void provideAuthentication(QString username, QString password);
	virtual void paste(QString data) = 0;
	virtual void paste(QHash<QString, QVariant> settings, QString data) = 0;

protected slots:
	virtual void requestFinished(QNetworkReply *reply);

protected:
	QSettings *settings;
	QNetworkAccessManager *manager;

	QByteArray buildPostData(QHash<QString, QString> &post);
};

#endif // BASEPASTESERVICE_H
