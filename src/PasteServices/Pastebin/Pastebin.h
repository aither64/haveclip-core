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

#ifndef PASTEBIN_H
#define PASTEBIN_H

#define PASTEBIN_API_URL_PASTE "http://pastebin.com/api/api_post.php"
#define PASTEBIN_API_URL_LOGIN "http://pastebin.com/api/api_login.php"
#define PASTEBIN_API_DEV_KEY "b0c73ed40ddc4a65025b48dcd1b8fefa"

#include "../BasePasteService.h"

class Pastebin : public BasePasteService
{
	Q_OBJECT
public:
	enum Exposure {
		Public,
		Unlisted,
		Private
	};

	static Language m_languages[];

	explicit Pastebin(QSettings *settings, QObject *parent = 0);
	PasteService type();
	QString internalName();
	QString label();
	void applySettings(QHash<QString, QVariant> s);
	QString name();
	Exposure exposure();
	QString lang();
	QString expiration();
	bool login();
	QString username();
	
signals:
	
public slots:
	void provideAuthentication(QString username, QString password);
	void paste(QString data);
	void paste(QHash<QString, QVariant> settings, QString data);
	
protected slots:
	void requestFinished(QNetworkReply *reply);

private:
	QString m_name;
	Exposure m_exposure;
	QString m_lang;
	QString m_expire;
	bool m_login;
	QString m_username;
	QString m_userKey;
	QHash<QString, QString> preparedPost;
	bool loggingIn;
	QString loginUsername;

private slots:
	void paste(QHash<QString, QString> &data, bool login, QString username = QString());
};

#endif // PASTEBIN_H
