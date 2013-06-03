#include <QtGui/QApplication>
#include <QTextCodec>
#include "HaveClip.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("HaveFun.cz");
	QCoreApplication::setOrganizationDomain("havefun.cz");
	QCoreApplication::setApplicationName("HaveClip");

	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

	QApplication a(argc, argv);

	HaveClip hc;
	
	return a.exec();
}
