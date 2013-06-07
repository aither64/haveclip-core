#ifndef CLIPBOARDCONTENT_H
#define CLIPBOARDCONTENT_H

#include <QMimeData>
#include <QIcon>

class ClipboardContent
{
public:
	struct Preview
	{
		QString path;
		int width;
		int height;

		~Preview();
	};

	QMimeData *mimeData;
	QString title;
	QString excerpt;
	QIcon icon;
	Preview *preview;
	QStringList formats;

	ClipboardContent(QMimeData *data);
	~ClipboardContent();
	void init();
	bool operator==(const ClipboardContent &other) const;

private:
	Preview* createItemPreview(QImage &img);
	void setTitle(QString &str);
};

#endif // CLIPBOARDCONTENT_H
