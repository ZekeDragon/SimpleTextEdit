/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** main.cpp
** Copyright (C) 2024 Ezekiel Oruven
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
** Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
** WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************************************************/
#include "mainwindow.hpp"

#include <algorithm>

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "buildinfo.hpp"

QCommandLineOption localeOp()
{
	return { "locale", QApplication::tr("Set the translation locale file to use.", "Core"), "code" };
}

void setupParser(QCommandLineParser &parser)
{
	parser.addPositionalArgument(QApplication::tr("files", "Core"),
	                             QApplication::tr("Files to open in the Text Editor.", "Core"),
	                             QApplication::tr("[files...]", "Core"));
	parser.addVersionOption();
	parser.addHelpOption();
	parser.setApplicationDescription(
	            QApplication::tr("The Simple Qt Text Editor is a clone of Windows Notepad for all major platforms.",
	                             "Core"));
	parser.addOption(localeOp());
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	const QStringList args = a.arguments();
	QTranslator translator;
	QStringList uiLanguages = QLocale::system().uiLanguages();
	if (auto localeLoc = std::find(args.begin(), args.end(), "--locale");
	    localeLoc != args.end() && ++localeLoc != args.end())
	{
		uiLanguages.push_front(*localeLoc);
	}

	for (const QString &locale : uiLanguages)
	{
		QLocale qLocale(locale);
		const QString baseName = "SimpleTextEdit_" + qLocale.name();
		if (translator.load(":/i18n/" + baseName))
		{
			a.installTranslator(&translator);
			QLocale::setDefault(qLocale);
			break;
		}
	}

	a.setApplicationName(QApplication::tr("Simple Qt Text Editor", "Core"));
	a.setApplicationVersion(versionString());
	a.setOrganizationName(QApplication::tr("KirHut Software Company", "Core"));
	a.setOrganizationDomain(QApplication::tr("kirhut.com", "Core"));
	QCommandLineParser parser;
	setupParser(parser);
	parser.process(args);
	MainWindow w;
	w.show();
	return a.exec();
}
