/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** mainwindow.cpp
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
#include "./ui_mainwindow.h"

#include <QProcess>
#include <QMessageBox>
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QCloseEvent>

#include "aboutdialog.hpp"
#include "findreplacedialog.hpp"

struct MainWindow::Impl
{
	Impl(MainWindow *top) :
	    top(top),
	    fDialog(top),
	    pDialog(&filePrinter, top),
	    psDialog(&filePrinter, top),
	    about(top),
	    findrep(top)
	{
		ui.setupUi(top);
		fDialog.setCurrentFont(ui.mainEdit->currentFont());
		document = ui.mainEdit->document();
		updateFileDisplay();
		QObject::connect(&findrep, SIGNAL(findRequested(FindFlags,QString)),
		                 top,      SLOT(doFindRequest(FindFlags,QString)));
		QObject::connect(&findrep, SIGNAL(replaceRequested(FindFlags,QString,QString)),
		                 top,      SLOT(doReplaceRequest(FindFlags,QString,QString)));
		QObject::connect(&findrep, SIGNAL(replaceAllRequested(FindFlags,QString,QString)),
		                 top,      SLOT(doReplaceAllRequest(FindFlags,QString,QString)));
		QObject::connect(top, SIGNAL(nothingToFind()), &findrep, SLOT(reportNoFind()));
	}

	void updateFileDisplay()
	{
		QString name = document->isModified() ? "*" : "";
		name += fileName.isEmpty() ? tr("Untitled") : fileName.section('/', -1);
		top->setWindowTitle(name.append(tr(" - Simple Qt Text Editor")));
	}

	bool editedCheck()
	{
		if (document->isModified())
		{
			auto response = QMessageBox::question(top, tr("Current File Has Been Modified"),
			                                      tr("The currently open file has been modified. "
			                                         "Do you want to save before closing it?"),
			                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			if (response == QMessageBox::Yes)
			{
				top->saveFile();
			}

			return response != QMessageBox::Cancel;
		}

		return true;
	}

	MainWindow *top;
	Ui::MainWindow ui;
	QString fileName;
	QPrinter filePrinter;
	QTextDocument *document;
	QFontDialog fDialog;
	QPrintDialog pDialog;
	QPageSetupDialog psDialog;
	AboutDialog about;
	FindReplaceDialog findrep;
	bool modCheck = false;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    im(std::make_unique<MainWindow::Impl>(this))
{
	// No implementation.
}

MainWindow::~MainWindow()
{
	// No implementation.
}


void MainWindow::newFile()
{
	if (im->editedCheck())
	{
		im->fileName.clear();
		im->ui.mainEdit->setText("");
		im->updateFileDisplay();
	}
}

void MainWindow::newWindow()
{
	QString appLoc = qApp->applicationFilePath();
	if (!QProcess::startDetached(appLoc))
	{
		QMessageBox::warning(this, tr("New Window Creation Failed"),
		                     tr("Creating a new Simple Qt Text Editor window failed."));
	}
}

void MainWindow::openFile()
{
	if (QString filename = QFileDialog::getOpenFileName(this, tr("Open Text File")); !filename.isNull())
	{
		if (im->editedCheck())
		{
			QFile fileToOpen(filename);
			if (fileToOpen.open(QIODeviceBase::ReadOnly))
			{
				QTextStream in(&fileToOpen);
				im->ui.mainEdit->setText(in.readAll());
				im->fileName = filename;
				im->document->setModified(false);
				im->modCheck = false;
				im->updateFileDisplay();
			}
			else
			{
				QMessageBox::critical(this, tr("File Failed to Open"),
				                      tr("Opening the selected file failed, the reason was not diagnosed."));
			}
		}
	}
}

void MainWindow::saveAs()
{
	if (QString filename = QFileDialog::getSaveFileName(this, tr("Save As...")); !filename.isNull())
	{
		QFile fileLoc(filename);
		if (fileLoc.exists())
		{
			if (QMessageBox::question(this, tr("File Already Exists"),
			                          tr("The filename selected already exists and this will overwrite it. "
			                             "Is it OK to overwrite?"))
			    == QMessageBox::No)
			{
				return;
			}
		}

		if (fileLoc.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate))
		{
			QTextStream out(&fileLoc);
			out << im->ui.mainEdit->toPlainText();
			im->document->setModified(false);
			im->modCheck = false;
			im->fileName = filename;
			im->updateFileDisplay();
		}
		else
		{
			QMessageBox::critical(this, tr("File Failed to Save"),
			                      tr("Saving the selected filename failed, the reason was not diagnosed."));
		}
	}
}

void MainWindow::saveFile()
{
	if (im->fileName.isNull())
	{
		saveAs();
	}
	else
	{
		QFile fileLoc(im->fileName);
		if (fileLoc.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate))
		{
			QTextStream out(&fileLoc);
			out << im->ui.mainEdit->toPlainText();
			im->document->setModified(false);
			im->modCheck = false;
			im->updateFileDisplay();
		}
		else
		{
			QMessageBox::critical(this, tr("File Failed to Save"),
			                      tr("Saving the currently open file failed, the reason was not diagnosed."));
		}
	}
}

void MainWindow::pageSetup()
{
	im->psDialog.show();
}

void MainWindow::printDialog()
{
	im->pDialog.open(this, SLOT(print()));
}

void MainWindow::deleteText()
{
	im->ui.mainEdit->textCursor().deleteChar();
}

void MainWindow::find()
{
	im->findrep.focusFind(true);
	im->findrep.show();
}

void MainWindow::replace()
{
	im->findrep.focusFind(false);
	im->findrep.show();
}

void MainWindow::timeDate()
{
	im->ui.mainEdit->textCursor().insertText(QDateTime::currentDateTime().toString(tr("hh:mm M/d/yyyy")));
}

void MainWindow::wordWrap(bool checked)
{
	im->ui.mainEdit->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void MainWindow::fontDialog()
{
	im->fDialog.open(this, SLOT(fontChanged(QFont const&)));
}

void MainWindow::onlineHelp()
{
	QDesktopServices::openUrl(QUrl("https://www.kirhut.com/docs/doku.php?id=weekend:project2"));
}

void MainWindow::aboutDialog()
{
	im->about.show();
}

void MainWindow::textChanged()
{
	if (im->modCheck != im->document->isModified())
	{
		im->modCheck = im->document->isModified();
		im->updateFileDisplay();
	}
}

void MainWindow::print()
{
	im->document->print(&im->filePrinter);
}

void MainWindow::fontChanged(const QFont &font)
{
	im->ui.mainEdit->setCurrentFont(font);
}

void MainWindow::doFindRequest(FindFlags flags, const QString &seek)
{
	emit nothingToFind();
}

void MainWindow::doReplaceRequest(FindFlags flags, const QString &seek, const QString &replace)
{
	emit nothingToFind();
}

void MainWindow::doReplaceAllRequest(FindFlags flags, const QString &seek, const QString &replace)
{
	emit nothingToFind();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (im->editedCheck())
	{
		QMainWindow::closeEvent(event);
	}
	else
	{
		event->ignore();
	}
}
