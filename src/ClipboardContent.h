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

#ifndef CLIPBOARDCONTENT_H
#define CLIPBOARDCONTENT_H

#include <QClipboard>
#include <QMimeData>
#include <QIcon>

class ClipboardContent
{
public:
	enum Mode {
		Selection,
		Clipboard,
		FindBuffer,
		ClipboardAndSelection
	};

	struct Preview
	{
		QString path;
		int width;
		int height;

		~Preview();
	};

	Mode mode;
	QMimeData *mimeData;
	QString title;
	QString excerpt;
	QIcon icon;
	Preview *preview;
	QStringList formats;

	ClipboardContent(Mode m, QMimeData *data);
	~ClipboardContent();
	void init();
	QString toPlainText();
	bool operator==(const ClipboardContent &other) const;
	bool operator!=(const ClipboardContent &other) const;
	static bool compareMimeData(const QMimeData *data1, const QMimeData *data2, bool isSelection);
	void save(QDataStream &ds) const;
	static ClipboardContent* load(QDataStream &ds);
	static Mode qtModeToOwn(QClipboard::Mode m);
	static QClipboard::Mode ownModeToQt(Mode m);

private:
	Preview* createItemPreview(QImage &img);
	void setTitle(QString &str);
	QString escape(QString str);
};

inline QDataStream& operator<<(QDataStream &ds, const ClipboardContent &cnt)
{
	cnt.save(ds);

	return ds;
}

#endif // CLIPBOARDCONTENT_H
