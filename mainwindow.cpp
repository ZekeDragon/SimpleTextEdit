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
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QLabel>

#include <tuple>
#include <array>

#include "aboutdialog.hpp"
#include "findreplacedialog.hpp"

constexpr size_t DEFAULT_ZOOM = 9;

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
		fDialog.setCurrentFont(ui.mainEdit->currentCharFormat().font());
		document = ui.mainEdit->document();
		ui.mainEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
		updateFileDisplay();
		lineColLabel.setMinimumWidth(120);
		zoomLabel.setMinimumWidth(60);
		lineEndLabel.setMinimumWidth(100);
		formatLabel.setMinimumWidth(100);
		updateLineColLabel();
		generateSlideRule();
		updateZoomLabel();
		lineEndLabel.setText("UNIX (LF)");
		formatLabel.setText("UTF-8");
		ui.statusbar->addPermanentWidget(new QLabel(""));
		ui.statusbar->addPermanentWidget(&lineColLabel);
		ui.statusbar->addPermanentWidget(&zoomLabel);
		ui.statusbar->addPermanentWidget(&lineEndLabel);
		ui.statusbar->addPermanentWidget(&formatLabel);
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
		name.append(fileName.isEmpty() ? tr("Untitled") : fileName.section('/', -1));
		top->setWindowTitle(name + tr(" - Simple Qt Text Editor"));
	}

	void updateLineColLabel()
	{
		QTextCursor current = ui.mainEdit->textCursor();
		QString lineSide = tr("Ln ") + QString::number(current.blockNumber());
		QString colSide = tr(", Col ") + QString::number(current.positionInBlock());
		lineColLabel.setText(lineSide + colSide);
	}

	template<typename Func>
	void doZoom(Func f)
	{
		if (size_t newZoom = f(currentZoom); newZoom < zoomSlideRule.size())
		{
			currentZoom = newZoom;
		}

		QFont newSize = document->defaultFont();
		newSize.setPointSizeF(zoomSlideRule[currentZoom]);
		document->setDefaultFont(newSize);
		updateZoomLabel();
	}

	void generateSlideRule()
	{
		qreal step = 0;
		const qreal basis = document->defaultFont().pointSizeF();
		for (qreal &sr : zoomSlideRule)
		{
			sr = basis * (step += .1);
		}
	}

	void updateZoomLabel()
	{
		int num = int((currentZoom + 1) * 10);
		//: Zoom level string, passed value will be between 10 and 500, in 10 point increments.
		zoomLabel.setText(tr("%n%", "MainWindow", num));
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

	void openFindReplace(bool findOrReplace)
	{
		findrep.focusFind(findOrReplace);
		if (QTextCursor content = ui.mainEdit->textCursor(); content.hasSelection())
		{
			findrep.setFindText(content.selectedText());
		}
		findrep.show();
	}

	std::tuple<QTextDocument::FindFlags, bool, bool> breakdownFindFlags(FindFlags flags)
	{
		auto qFlags = QFlags<QTextDocument::FindFlag>::fromInt(flags.to_ulong() & 0b111);
		return { qFlags, flags.test(3), flags.test(4) };
	}

	QTextCursor findNext(FindFlags flags, QString const &seek)
	{
		return findNext(flags, seek, ui.mainEdit->textCursor());
	}

	QTextCursor findNext(FindFlags flags, QString const &seek, QTextCursor const &startPos)
	{
		auto [findflag, isRegex, shouldWrap] = breakdownFindFlags(flags);
		auto findStr = [&](auto f, auto fPos, bool regx) {
			return regx ? document->find(QRegularExpression(seek), fPos, f)
			            : document->find(seek, fPos, f);
		};

		QTextCursor select = findStr(findflag, startPos, isRegex);
		if (select.isNull() && shouldWrap)
		{
			int from = (findflag & 1) ? document->characterCount() : 0;
			select = findStr(findflag, from, isRegex);
		}

		return select;
	}

	bool doFindRequest(FindFlags flags, QString const &seek)
	{
		if (QTextCursor select = findNext(flags, seek); !select.isNull())
		{
			ui.mainEdit->setTextCursor(select);
			return true;
		}

		return false;
	}

	MainWindow *top;
	Ui::MainWindow ui;
	std::array<qreal, 50> zoomSlideRule;
	size_t currentZoom = DEFAULT_ZOOM;
	QString fileName;
	QPrinter filePrinter;
	QTextDocument *document;
	QFontDialog fDialog;
	QPrintDialog pDialog;
	QPageSetupDialog psDialog;
	QLabel lineColLabel, zoomLabel, lineEndLabel, formatLabel;
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
		im->document->setPlainText("");
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
				im->document->setPlainText(in.readAll());
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
	im->psDialog.open();
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
	im->openFindReplace(true);
}

void MainWindow::replace()
{
	im->openFindReplace(false);
}

void MainWindow::timeDate()
{
	im->ui.mainEdit->textCursor().insertText(QDateTime::currentDateTime().toString(tr("hh:mm M/d/yyyy")));
}

void MainWindow::wordWrap(bool checked)
{
	im->ui.mainEdit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void MainWindow::fontDialog()
{
	im->fDialog.open(this, SLOT(fontChanged(QFont const&)));
}

void MainWindow::onlineHelp()
{
	QDesktopServices::openUrl(QUrl(tr("https://www.kirhut.com/docs/doku.php?id=weekend:project2")));
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

void MainWindow::cursorMoved()
{
	im->updateLineColLabel();
}

void MainWindow::zoomIn()
{
	im->doZoom([](size_t current) { return current + 1; });
}

void MainWindow::zoomOut()
{
	im->doZoom([](size_t current) { return current - 1; });
}

void MainWindow::restoreZoom()
{
	im->doZoom([]([[maybe_unused]] auto _) { return DEFAULT_ZOOM; });
}

void MainWindow::print()
{
	im->document->print(&im->filePrinter);
}

void MainWindow::fontChanged(const QFont &font)
{
	im->document->setDefaultFont(font);
	im->generateSlideRule();
	im->currentZoom = DEFAULT_ZOOM;
}

void MainWindow::doFindRequest(FindFlags flags, const QString &seek)
{
	if (!im->doFindRequest(flags, seek))
	{
		emit nothingToFind();
	}
}

void MainWindow::doReplaceRequest(FindFlags flags, const QString &seek, const QString &replace)
{
	QTextCursor current = im->ui.mainEdit->textCursor();
	bool reportFail = true;
	if (current.hasSelection())
	{
		current.insertText(replace);
		reportFail = false;
	}

	if (!im->doFindRequest(flags, seek) && reportFail)
	{
		emit nothingToFind();
	}
}

void MainWindow::doReplaceAllRequest(FindFlags flags, const QString &seek, const QString &replace)
{
	QTextCursor atFront = im->ui.mainEdit->textCursor();
	atFront.setPosition(0);
	bool found = false;
	// Replace All does not want to wrap around, so ignore if the user has it set.
	flags.set(4, false);
	while (!(atFront = im->findNext(flags, seek, atFront)).isNull())
	{
		found = true;
		atFront.insertText(replace);
	}

	if (!found)
	{
		emit nothingToFind();
	}
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
