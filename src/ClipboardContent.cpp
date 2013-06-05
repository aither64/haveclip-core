#include <QTextDocument>
#include <QUrl>
#include <QStringList>
#include <QFile>
#include <QTemporaryFile>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QDebug>

#include "ClipboardContent.h"

ClipboardContent::Preview::~Preview()
{
	QFile::remove(path);
}

ClipboardContent::ClipboardContent(QMimeData *data) :
	preview(0),
	mimeData(data)
{
	foreach(QString f, mimeData->formats())
	{
		if(f.indexOf('/') != -1)
			formats << f;
	}
}

void ClipboardContent::init()
{
	QString tmp;

	if(mimeData->hasHtml())
	{
		tmp = mimeData->html();
		excerpt = Qt::escape(tmp.left(200));

		if(tmp.count() > 200)
			excerpt += "<br>...";

	} else if(mimeData->hasText()) {
		tmp = mimeData->text();
		excerpt = Qt::escape(tmp.left(200));

		if(tmp.count() > 200)
			excerpt += "<br>...";

	} else if(mimeData->hasUrls()) {
		QList<QUrl> urls = mimeData->urls();

		foreach(QUrl u, urls.mid(0, 5))
			excerpt += u.toString() + "<br>";

		if(urls.size() > 5)
			excerpt += "<br>...";

		if(urls.size() > 0)
			tmp = urls.first().toString();

	} else if(mimeData->hasImage()) {
		// For some reason, QMimeData::imageData().value<QImage>() does not work on copied QMimeData
		// Loaded image is null
		QImage img;
		img.loadFromData(mimeData->data("application/x-qt-image"));

		excerpt = tmp = QObject::tr("Image");
		icon = QIcon( QPixmap::fromImage(img) );

#ifdef Q_OS_LINUX
		preview = createItemPreview(img);

		if(preview)
		{
			QString prop;

			if(preview->width > 400)
				prop = QString("width=\"%1\"").arg(400);
			else if(preview->height > 400)
				prop = QString("height=\"%1\"").arg(400);

			excerpt += QString("<br><img src=\"%1\" %2>").arg(preview->path).arg(prop);
		}
#endif
	}

	if(mimeData->hasText())
		title = mimeData->text().trimmed().left(30);
	else
		title = tmp.trimmed().left(30);
}

ClipboardContent::~ClipboardContent()
{
	if(preview)
		delete preview;

	delete mimeData;
}

bool ClipboardContent::operator==(const ClipboardContent &other) const
{
	if(formats != other.formats)
		return false;

	foreach(QString f, formats)
	{
		if(mimeData->data(f) != other.mimeData->data(f))
			return false;
	}

	return true;
}

ClipboardContent::Preview* ClipboardContent::createItemPreview(QImage &img)
{
	Preview *preview = 0;
	QTemporaryFile tmp(QDir::tempPath() + "/haveclip-preview-XXXXXX");
	tmp.setAutoRemove(false);

	if(tmp.open())
	{
		if( img.save(&tmp, "PNG") )
		{
			preview = new Preview;
			preview->path = tmp.fileName();
			preview->width = img.width();
			preview->height = img.height();
		}

		tmp.close();
	}

	return preview;
}
