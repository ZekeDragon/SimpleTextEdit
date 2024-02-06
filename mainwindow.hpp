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
#pragma once

#include <QMainWindow>

#include <memory>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

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


private slots:
	void print();
	void fontChanged(QFont const &font);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	struct Impl;
	std::unique_ptr<Impl> im;
};
