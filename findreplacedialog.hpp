/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** findreplacedialog.hpp
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

#include <QDialog>

#include <memory>

#include "findflags.hpp"

class FindReplaceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FindReplaceDialog(QWidget *parent = nullptr);
	~FindReplaceDialog();

	void focusFind(bool findOrReplace);

signals:
	void findRequested(FindFlags flags, QString const &seek);
	void replaceRequested(FindFlags flags, QString const &seek, QString const &replace);
	void replaceAllRequested(FindFlags flags, QString const &seek, QString const &replace);

public slots:
	void findFieldChanged(QString const &newText);
	void replaceFieldChanged(QString const &newText);

	void regexToggled(bool checked);

	void findNextPressed();
	void replacePressed();
	void replaceAllPressed();

	void reportNoFind();

private slots:
	void silenceNotFound();

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

private:
	struct Impl;
	std::unique_ptr<Impl> im;
};

