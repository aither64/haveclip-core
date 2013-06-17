#include <QDebug>
#include <QStringList>

#include "Pastebin.h"

BasePasteService::Language Pastebin::m_languages[] = {
	{"4cs", "4CS"},
	{"6502acme", "6502 ACME Cross Assembler"},
	{"6502kickass", "6502 Kick Assembler"},
	{"6502tasm", "6502 TASM/64TASS"},
	{"abap", "ABAP"},
	{"actionscript", "ActionScript"},
	{"actionscript3", "ActionScript 3"},
	{"ada", "Ada"},
	{"algol68", "ALGOL 68"},
	{"apache", "Apache Log"},
	{"applescript", "AppleScript"},
	{"apt_sources", "APT Sources"},
	{"arm", "ARM"},
	{"asm", "ASM (NASM)"},
	{"asp", "ASP"},
	{"asymptote", "Asymptote"},
	{"autoconf", "autoconf"},
	{"autohotkey", "Autohotkey"},
	{"autoit", "AutoIt"},
	{"avisynth", "Avisynth"},
	{"awk", "Awk"},
	{"bascomavr", "BASCOM AVR"},
	{"bash", "Bash"},
	{"basic4gl", "Basic4GL"},
	{"bibtex", "BibTeX"},
	{"blitzbasic", "Blitz Basic"},
	{"bnf", "BNF"},
	{"boo", "BOO"},
	{"bf", "BrainFuck"},
	{"c", "C"},
	{"c_mac", "C for Macs"},
	{"cil", "C Intermediate Language"},
	{"csharp", "C#"},
	{"cpp", "C++"},
	{"cpp-qt", "C++ (with QT extensions)"},
	{"c_loadrunner", "C: Loadrunner"},
	{"caddcl", "CAD DCL"},
	{"cadlisp", "CAD Lisp"},
	{"cfdg", "CFDG"},
	{"chaiscript", "ChaiScript"},
	{"clojure", "Clojure"},
	{"klonec", "Clone C"},
	{"klonecpp", "Clone C++"},
	{"cmake", "CMake"},
	{"cobol", "COBOL"},
	{"coffeescript", "CoffeeScript"},
	{"cfm", "ColdFusion"},
	{"css", "CSS"},
	{"cuesheet", "Cuesheet"},
	{"d", "D"},
	{"dcl", "DCL"},
	{"dcpu16", "DCPU-16"},
	{"dcs", "DCS"},
	{"delphi", "Delphi"},
	{"oxygene", "Delphi Prism (Oxygene)"},
	{"diff", "Diff"},
	{"div", "DIV"},
	{"dos", "DOS"},
	{"dot", "DOT"},
	{"e", "E"},
	{"ecmascript", "ECMAScript"},
	{"eiffel", "Eiffel"},
	{"email", "Email"},
	{"epc", "EPC"},
	{"erlang", "Erlang"},
	{"fsharp", "F#"},
	{"falcon", "Falcon"},
	{"fo", "FO Language"},
	{"f1", "Formula One"},
	{"fortran", "Fortran"},
	{"freebasic", "FreeBasic"},
	{"freeswitch", "FreeSWITCH"},
	{"gambas", "GAMBAS"},
	{"gml", "Game Maker"},
	{"gdb", "GDB"},
	{"genero", "Genero"},
	{"genie", "Genie"},
	{"gettext", "GetText"},
	{"go", "Go"},
	{"groovy", "Groovy"},
	{"gwbasic", "GwBasic"},
	{"haskell", "Haskell"},
	{"haxe", "Haxe"},
	{"hicest", "HicEst"},
	{"hq9plus", "HQ9 Plus"},
	{"html4strict", "HTML"},
	{"html5", "HTML 5"},
	{"icon", "Icon"},
	{"idl", "IDL"},
	{"ini", "INI file"},
	{"inno", "Inno Script"},
	{"intercal", "INTERCAL"},
	{"io", "IO"},
	{"j", "J"},
	{"java", "Java"},
	{"java5", "Java 5"},
	{"javascript", "JavaScript"},
	{"jquery", "jQuery"},
	{"kixtart", "KiXtart"},
	{"latex", "Latex"},
	{"ldif", "LDIF"},
	{"lb", "Liberty BASIC"},
	{"lsl2", "Linden Scripting"},
	{"lisp", "Lisp"},
	{"llvm", "LLVM"},
	{"locobasic", "Loco Basic"},
	{"logtalk", "Logtalk"},
	{"lolcode", "LOL Code"},
	{"lotusformulas", "Lotus Formulas"},
	{"lotusscript", "Lotus Script"},
	{"lscript", "LScript"},
	{"lua", "Lua"},
	{"m68k", "M68000 Assembler"},
	{"magiksf", "MagikSF"},
	{"make", "Make"},
	{"mapbasic", "MapBasic"},
	{"matlab", "MatLab"},
	{"mirc", "mIRC"},
	{"mmix", "MIX Assembler"},
	{"modula2", "Modula 2"},
	{"modula3", "Modula 3"},
	{"68000devpac", "Motorola 68000 HiSoft Dev"},
	{"mpasm", "MPASM"},
	{"mxml", "MXML"},
	{"mysql", "MySQL"},
	{"nagios", "Nagios"},
	{"newlisp", "newLISP"},
	{"text", "None"},
	{"nsis", "NullSoft Installer"},
	{"oberon2", "Oberon 2"},
	{"objeck", "Objeck Programming Langua"},
	{"objc", "Objective C"},
	{"ocaml-brief", "OCalm Brief"},
	{"ocaml", "OCaml"},
	{"octave", "Octave"},
	{"pf", "OpenBSD PACKET FILTER"},
	{"glsl", "OpenGL Shading"},
	{"oobas", "Openoffice BASIC"},
	{"oracle11", "Oracle 11"},
	{"oracle8", "Oracle 8"},
	{"oz", "Oz"},
	{"parasail", "ParaSail"},
	{"parigp", "PARI/GP"},
	{"pascal", "Pascal"},
	{"pawn", "PAWN"},
	{"pcre", "PCRE"},
	{"per", "Per"},
	{"perl", "Perl"},
	{"perl6", "Perl 6"},
	{"php", "PHP"},
	{"php-brief", "PHP Brief"},
	{"pic16", "Pic 16"},
	{"pike", "Pike"},
	{"pixelbender", "Pixel Bender"},
	{"plsql", "PL/SQL"},
	{"postgresql", "PostgreSQL"},
	{"povray", "POV-Ray"},
	{"powershell", "Power Shell"},
	{"powerbuilder", "PowerBuilder"},
	{"proftpd", "ProFTPd"},
	{"progress", "Progress"},
	{"prolog", "Prolog"},
	{"properties", "Properties"},
	{"providex", "ProvideX"},
	{"purebasic", "PureBasic"},
	{"pycon", "PyCon"},
	{"python", "Python"},
	{"pys60", "Python for S60"},
	{"q", "q/kdb+"},
	{"qbasic", "QBasic"},
	{"rsplus", "R"},
	{"rails", "Rails"},
	{"rebol", "REBOL"},
	{"reg", "REG"},
	{"rexx", "Rexx"},
	{"robots", "Robots"},
	{"rpmspec", "RPM Spec"},
	{"ruby", "Ruby"},
	{"gnuplot", "Ruby Gnuplot"},
	{"sas", "SAS"},
	{"scala", "Scala"},
	{"scheme", "Scheme"},
	{"scilab", "Scilab"},
	{"sdlbasic", "SdlBasic"},
	{"smalltalk", "Smalltalk"},
	{"smarty", "Smarty"},
	{"spark", "SPARK"},
	{"sparql", "SPARQL"},
	{"sql", "SQL"},
	{"stonescript", "StoneScript"},
	{"systemverilog", "SystemVerilog"},
	{"tsql", "T-SQL"},
	{"tcl", "TCL"},
	{"teraterm", "Tera Term"},
	{"thinbasic", "thinBasic"},
	{"typoscript", "TypoScript"},
	{"unicon", "Unicon"},
	{"uscript", "UnrealScript"},
	{"ups", "UPC"},
	{"urbi", "Urbi"},
	{"vala", "Vala"},
	{"vbnet", "VB.NET"},
	{"vedit", "Vedit"},
	{"verilog", "VeriLog"},
	{"vhdl", "VHDL"},
	{"vim", "VIM"},
	{"visualprolog", "Visual Pro Log"},
	{"vb", "VisualBasic"},
	{"visualfoxpro", "VisualFoxPro"},
	{"whitespace", "WhiteSpace"},
	{"whois", "WHOIS"},
	{"winbatch", "Winbatch"},
	{"xbasic", "XBasic"},
	{"xml", "XML"},
	{"xorg_conf", "Xorg Config"},
	{"xpp", "XPP"},
	{"yaml", "YAML"},
	{"z80", "Z80 Assembler"},
	{"zxbasic", "ZXBasic"},
	{0, 0}
};

Pastebin::Pastebin(QSettings *settings, QObject *parent) :
	BasePasteService(settings, parent),
	loggingIn(false)
{
	settings->beginGroup("PasteServices/Pastebin");

	m_name = settings->value("Name").toString();
	m_exposure = (Pastebin::Exposure) settings->value("Exposure", Pastebin::Unlisted).toInt();
	m_lang = settings->value("Language", "text").toString();
	m_expire = settings->value("Expiration", "1H").toString();
	m_login = settings->value("Login", false).toBool();
	m_username = settings->value("Username").toString();
	m_userKey = settings->value("UserKey").toString();

	settings->endGroup();
}

BasePasteService::PasteService Pastebin::type()
{
	return BasePasteService::Pastebin;
}

QString Pastebin::internalName()
{
	return "Pastebin";
}

QString Pastebin::label()
{
	return "Pastebin.com";
}

void Pastebin::applySettings(QHash<QString, QVariant> s)
{
	settings->beginGroup("PasteServices/Pastebin");

	m_name = s["Name"].toString();
	m_exposure = (Pastebin::Exposure) s["Exposure"].toInt();
	m_lang = s["Lang"].toString();
	m_expire = s["Expire"].toString();
	m_login = s["Login"].toBool();
	m_username = s["Username"].toString();

	settings->setValue("Name", m_name);
	settings->setValue("Exposure", m_exposure);
	settings->setValue("Language", m_lang);
	settings->setValue("Expiration", m_expire);
	settings->setValue("Login", m_login);
	settings->setValue("Username", m_username);

	settings->endGroup();
}

QString Pastebin::name()
{
	return m_name;
}

Pastebin::Exposure Pastebin::exposure()
{
	return m_exposure;
}

QString Pastebin::lang()
{
	return m_lang;
}

QString Pastebin::expiration()
{
	return m_expire;
}

bool Pastebin::login()
{
	return m_login;
}

QString Pastebin::username()
{
	return m_username;
}

void Pastebin::provideAuthentication(QString username, QString password)
{
	QNetworkRequest request(QUrl(PASTEBIN_API_URL_LOGIN));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	QHash<QString, QString> post;
	post["api_dev_key"] = PASTEBIN_API_DEV_KEY;
	post["api_user_name"] = username;
	post["api_user_password"] = password;

	manager->post(request, buildPostData(post));

	loggingIn = true;
}

void Pastebin::paste(QString data)
{
	QHash<QString, QString> post;
	post["api_paste_code"] = data;
	post["api_paste_name"] = m_name;
	post["api_paste_format"] = m_lang;
	post["api_paste_private"] = QString::number(m_exposure);
	post["api_paste_expire_date"] = m_expire;

	paste(post, m_login, m_username);
}

void Pastebin::paste(QHash<QString, QVariant> settings, QString data)
{
	QHash<QString, QString> post;
	post["api_paste_code"] = data;
	post["api_paste_name"] = settings["Name"].toString();
	post["api_paste_format"] = settings["Lang"].toString();
	post["api_paste_private"] = QString::number(settings["Exposure"].toInt());
	post["api_paste_expire_date"] = settings["Expire"].toString();

	paste(post, settings["Login"].toBool(), settings["Username"].toString());
}

void Pastebin::paste(QHash<QString, QString> &post, bool login, QString username)
{
	post["api_dev_key"] = PASTEBIN_API_DEV_KEY;
	post["api_option"] = "paste";

	if(login)
	{
		qDebug() << "Pastebin login" << m_userKey << username;
		if(m_userKey.isEmpty())
		{
			preparedPost = post;
			loginUsername = username;
			emit authenticationRequired(username, false, "");
			return;
		}

		post["api_user_key"] = m_userKey;
	}

	QNetworkRequest request(QUrl(PASTEBIN_API_URL_PASTE));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	manager->post(request, buildPostData(post));
}

void Pastebin::requestFinished(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		qDebug() << "Error pasting to pastebin" << reply->error();
		emit errorOccured(reply->errorString());
		return;
	}

	QString ret = QString(reply->readAll()).trimmed();

	qDebug() << "pastebin.com says" << ret;

	if(ret.startsWith("Bad API"))
	{
		// handle error
		qDebug() << "Error" << ret;

		if(loggingIn)
			emit authenticationRequired(loginUsername, true, ret.mid(ret.indexOf(',')+1, -1).trimmed());
		else
			emit errorOccured(reply->errorString());
		return;
	}

	if(loggingIn)
	{
		m_userKey = ret;
		settings->setValue("PasteServices/Pastebin/UserKey", m_userKey);
		loggingIn = false;

		paste(preparedPost, true);

	} else {
		qDebug() << "Paste link" << ret;

		if(!ret.startsWith("Error"))
			emit pasted(QUrl(ret));
	}

	reply->deleteLater();
}
