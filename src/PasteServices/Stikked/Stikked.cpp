#include <QDebug>

#include "Stikked.h"

Stikked::Language Stikked::languages[] = {
	{"html5", "HTML5"},
	{"css", "CSS"},
	{"javascript", "JavaScript"},
	{"php", "PHP"},
	{"python", "Python"},
	{"ruby", "Ruby"},
	{"lua", "Lua"},
	{"bash", "Bash"},
	{"erlang", "Erlang"},
	{"go", "Go"},
	{"c", "C"},
	{"cpp", "C++"},
	{"diff", "Diff-output"},
	{"latex", "LaTeX"},
	{"sql", "SQL"},
	{"xml", "XML"},
	{"text", "Plain Text"},
	{"4cs", "4CS"},
	{"6502acme", "MOS 6502"},
	{"6502kickass", "MOS 6502 Kick Assembler"},
	{"6502tasm", "MOS 6502 TASM/64TASS"},
	{"68000devpac", "Motorola 68000 Devpac Assembler"},
	{"abap", "ABAP"},
	{"actionscript", "Actionscript"},
	{"actionscript3", "ActionScript3"},
	{"ada", "Ada"},
	{"algol68", "ALGOL 68"},
	{"apache", "Apache"},
	{"applescript", "AppleScript"},
	{"apt_sources", "Apt sources.list"},
	{"asm", "x86 Assembler"},
	{"asp", "ASP"},
	{"autoconf", "autoconf"},
	{"autohotkey", "Autohotkey"},
	{"autoit", "AutoIT"},
	{"avisynth", "AviSynth"},
	{"awk", "Awk"},
	{"bascomavr", "BASCOM AVR"},
	{"basic4gl", "Basic4GL"},
	{"bf", "Brainfuck"},
	{"bibtex", "BibTeX"},
	{"blitzbasic", "BlitzBasic"},
	{"bnf", "BNF (Backus-Naur form)"},
	{"boo", "Boo"},
	{"c_loadrunner", "C (for LoadRunner)"},
	{"c_mac", "C for Macs"},
	{"caddcl", "CAD DCL (Dialog Control Language)"},
	{"cadlisp", "AutoCAD/IntelliCAD Lisp"},
	{"cfdg", "CFDG"},
	{"cfm", "ColdFusion"},
	{"chaiscript", "ChaiScript"},
	{"cil", "CIL (Common Intermediate Language)"},
	{"clojure", "Clojure"},
	{"cmake", "CMake"},
	{"cobol", "COBOL"},
	{"coffeescript", "CoffeeScript"},
	{"csharp", "C#"},
	{"cuesheet", "Cuesheet"},
	{"d", "D"},
	{"dcs", "DCS"},
	{"delphi", "Delphi (Object Pascal)"},
	{"div", "DIV"},
	{"dos", "DOS"},
	{"dot", "dot"},
	{"e", "E"},
	{"ecmascript", "ECMAScript"},
	{"eiffel", "Eiffel"},
	{"email", "Email (mbox/eml/RFC format)"},
	{"epc", "Enerscript"},
	{"euphoria", "Euphoria"},
	{"f1", "Formula One"},
	{"falcon", "Falcon"},
	{"fo", "fo"},
	{"fortran", "Fortran"},
	{"freebasic", "FreeBasic"},
	{"fsharp", "F#"},
	{"gambas", "GAMBAS"},
	{"gdb", "GDB"},
	{"genero", "Genero"},
	{"genie", "Genie"},
	{"gettext", "GNU Gettext .po/.pot"},
	{"glsl", "glSlang"},
	{"gml", "GML"},
	{"gnuplot", "Gnuplot script"},
	{"groovy", "Groovy"},
	{"gwbasic", "GwBasic"},
	{"haskell", "Haskell"},
	{"hicest", "HicEst"},
	{"hq9plus", "HQ9+"},
	{"html4strict", "HTML 4.01 strict"},
	{"icon", "Icon"},
	{"idl", "Unoidl"},
	{"ini", "INI"},
	{"inno", "Inno Script"},
	{"intercal", "INTERCAL"},
	{"io", "Io"},
	{"j", "J"},
	{"java", "Java"},
	{"java5", "Java 5"},
	{"jquery", "jQuery 1.3"},
	{"klonec", "KLone with C"},
	{"klonecpp", "KLone with C++"},
	{"lb", "Liberty BASIC"},
	{"lisp", "Generic Lisp"},
	{"llvm", "LLVM"},
	{"locobasic", "Locomotive Basic (Amstrad CPC series)"},
	{"logtalk", "Logtalk"},
	{"lolcode", "LOLcode"},
	{"lotusformulas", "@Formula/@Command"},
	{"lotusscript", "LotusScript"},
	{"lscript", "Lightwave Script"},
	{"lsl2", "Linden Scripting"},
	{"m68k", "Motorola 68000 Assembler"},
	{"magiksf", "MagikSF"},
	{"make", "Make"},
	{"mapbasic", "MapBasic"},
	{"matlab", "Matlab M-file"},
	{"mirc", "mIRC Scripting"},
	{"mmix", "MMIX Assembler"},
	{"modula2", "Modula-2"},
	{"modula3", "Modula-3"},
	{"mpasm", "Microchip Assembler"},
	{"mxml", "MXML"},
	{"mysql", "MySQL"},
	{"newlisp", "newLISP"},
	{"nsis", "Nullsoft Scriptable Install System"},
	{"oberon2", "Oberon-2"},
	{"objc", "Objective-C"},
	{"objeck", "Objeck Programming Language"},
	{"ocaml", "OCaml (Objective Caml)"},
	{"oobas", "OpenOffice.org Basic"},
	{"oracle11", "Oracle 11i"},
	{"oracle8", "Oracle 8"},
	{"oxygene", "Delphi Prism (Oxygene)"},
	{"oz", "Oz"},
	{"pascal", "Pascal"},
	{"pcre", "PCRE"},
	{"per", "Per (forms)"},
	{"perl", "Perl"},
	{"perl6", "Perl 6"},
	{"pf", "OpenBSD packet filter"},
	{"pic16", "PIC16 Assembler"},
	{"pike", "Pike"},
	{"pixelbender", "Pixel Bender 1.0"},
	{"pli", "PL/I"},
	{"plsql", "Oracle 9.2 PL/SQL"},
	{"postgresql", "PostgreSQL"},
	{"povray", "Povray"},
	{"powerbuilder", "PowerBuilder (PowerScript)"},
	{"powershell", "PowerShell"},
	{"proftpd", "ProFTPd"},
	{"progress", "Progress"},
	{"prolog", "Prolog"},
	{"properties", "Property"},
	{"providex", "ProvideX"},
	{"purebasic", "PureBasic"},
	{"q", "q/kdb+"},
	{"qbasic", "QBasic/QuickBASIC"},
	{"rails", "Ruby (with Ruby on Rails Framework)"},
	{"rebol", "Rebol"},
	{"reg", "Microsoft Registry Editor"},
	{"robots", "robots.txt"},
	{"rpmspec", "RPM Spec"},
	{"rsplus", "R"},
	{"sas", "SAS"},
	{"scala", "Scala"},
	{"scheme", "Scheme"},
	{"scilab", "SciLab"},
	{"sdlbasic", "sdlBasic"},
	{"smalltalk", "Smalltalk"},
	{"smarty", "Smarty template"},
	{"systemverilog", "SystemVerilog IEEE 1800-2009(draft8)"},
	{"tcl", "TCL/iTCL"},
	{"teraterm", "Tera Term Macro"},
	{"thinbasic", "thinBasic"},
	{"tsql", "T-SQL"},
	{"typoscript", "TypoScript"},
	{"unicon", "Unicon"},
	{"uscript", "UnrealScript"},
	{"vala", "Vala"},
	{"vb", "Visual Basic"},
	{"vbnet", "VB.NET"},
	{"verilog", "Verilog"},
	{"vhdl", "VHDL"},
	{"vim", "Vim scripting"},
	{"visualfoxpro", "Visual FoxPro"},
	{"visualprolog", "Visual Prolog"},
	{"whitespace", "Whitespace"},
	{"whois", "Whois response (RPSL format)"},
	{"winbatch", "WinBatch"},
	{"xbasic", "XBasic"},
	{"xorg_conf", "xorg.conf"},
	{"xpp", "Axapta/Dynamics Ax X++"},
	{"yaml", "YAML"},
	{"z80", "ZiLOG Z80 Assembler"},
	{"zxbasic", "ZXBasic"},
	{0, 0}
};

Stikked::Stikked(QSettings *settings, QObject *parent) :
	BasePasteService(settings, parent)
{
	settings->beginGroup("PasteServices/Stikked");

	m_url = settings->value("Url").toString();
	m_name = settings->value("Name").toString();
	m_title = settings->value("Title").toString();
	m_privatePaste = settings->value("Private", true).toBool();
	m_lang = settings->value("Language", "text").toString();
	m_expire = settings->value("Expiration", 0).toInt();

	settings->endGroup();
}

BasePasteService::PasteService Stikked::type()
{
	return BasePasteService::Stikked;
}

QString Stikked::internalName()
{
	return "Stikked";
}

QString Stikked::label()
{
	return internalName();
}

void Stikked::applySettings(QHash<QString, QVariant> s)
{
	settings->beginGroup("PasteServices/Stikked");

	m_url = s["Url"].toString();
	m_name = s["Name"].toString();
	m_title = s["Title"].toString();
	m_privatePaste = s["Private"].toBool();
	m_lang = s["Lang"].toString();
	m_expire = s["Expire"].toInt();

	settings->setValue("Url", m_url);
	settings->setValue("Name", m_name);
	settings->setValue("Title", m_title);
	settings->setValue("Private", m_privatePaste);
	settings->setValue("Language", m_lang);
	settings->setValue("Expiration", m_expire);

	settings->endGroup();
}

QString Stikked::url()
{
	return m_url;
}

QString Stikked::name()
{
	return m_name;
}

QString Stikked::title()
{
	return m_title;
}

bool Stikked::isPrivate()
{
	return m_privatePaste;
}

QString Stikked::lang()
{
	return m_lang;
}

int Stikked::expiration()
{
	return m_expire;
}

void Stikked::paste(QString data)
{
	QHash<QString, QString> post;
	post["name"] = m_name;
	post["title"] = m_title;
	post["private"] = m_privatePaste ? "1" : "0";
	post["lang"] = m_lang;
	post["expire"] = QString::number(m_expire);
	post["text"] = data;

	paste(post);
}

void Stikked::paste(QHash<QString, QVariant> settings, QString data)
{
	QHash<QString, QString> post;
	post["name"] = settings["Name"].toString();
	post["title"] = settings["Title"].toString();
	post["private"] = settings["Private"].toBool() ? "1" : "0";
	post["lang"] = settings["Lang"].toString();
	post["expire"] = QString::number(settings["Expire"].toInt());
	post["text"] = data;

	paste(post);
}

void Stikked::paste(QHash<QString, QString> &post)
{
	QNetworkRequest request(m_url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	manager->post(request, buildPostData(post));
}

void Stikked::requestFinished(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		qDebug() << "Error pasting to stikked" << reply->error();
		return;
	}

	QString ret = QString(reply->readAll()).trimmed();

	qDebug() << "Paste link" << ret;

	if(!ret.startsWith("Error"))
		emit pasted(QUrl(ret));

	reply->deleteLater();
}

QByteArray Stikked::buildPostData(QHash<QString, QString> &data)
{
	QString ret;

	QHashIterator<QString, QString> i(data);
	bool first = true;

	while(i.hasNext())
	{
	    i.next();

	    if(first)
		    first = false;
	    else
		    ret += "&";

	    ret += i.key() + "=" + i.value();
	}

	return ret.toUtf8();
}
