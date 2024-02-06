/***********************************************************************************************************************
** {{ project }}
** %{Cpp:License:FileName}
** Copyright (C) 2023 KirHut Security Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
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

struct MainWindow::Impl
{
	Impl(MainWindow *top) :
	    top(top),
	    fDialog(top),
	    pDialog(&filePrinter, top),
	    psDialog(&filePrinter, top),
	    about(top)
	{
		ui.setupUi(top);
		fDialog.setCurrentFont(ui.mainEdit->currentFont());
		document = ui.mainEdit->document();
		updateFileDisplay();
	}

	void updateFileDisplay()
	{
		QString name = document->isModified() ? "*" : "";
		if (fileName.isNull() || fileName.isEmpty())
		{
			name += tr("Untitled");
		}
		else
		{
			name += fileName.section('/', -1);
		}

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

}

void MainWindow::replace()
{

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
