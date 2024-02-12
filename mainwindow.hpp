/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** mainwindow.hpp
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
#pragma once

#include <QMainWindow>

#include <memory>

#include "findflags.hpp"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

signals:
	void nothingToFind();

public slots:
	void newFile();
	void newWindow();
	void openFile();
	void saveAs();
	void saveFile();
	void pageSetup();
	void printDialog();

	void deleteText();

	void find();
	void replace();

	void timeDate();

	void wordWrap(bool checked);
	void fontDialog();

	void onlineHelp();
	void aboutDialog();

	void textChanged();
	void cursorMoved();

	void zoomIn();
	void zoomOut();
	void restoreZoom();

private slots:
	void print();
	void fontChanged(QFont const &font);

	void doFindRequest(FindFlags flags, QString const &seek);
	void doReplaceRequest(FindFlags flags, QString const &seek, QString const &replace);
	void doReplaceAllRequest(FindFlags flags, QString const &seek, QString const &replace);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	struct Impl;
	std::unique_ptr<Impl> im;
};
