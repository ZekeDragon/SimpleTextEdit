/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** aboutdialog.cpp
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
#include "aboutdialog.hpp"
#include "ui_aboutdialog.h"

#include "buildinfo.hpp"

struct AboutDialog::Impl
{
	Impl(AboutDialog *top) : top(top)
	{
		ui.setupUi(top);
		ui.mainNameLabel->setText(projectName());
		ui.versionLabel->setText(ui.versionLabel->text() + " " + versionString());
	}

	AboutDialog *top;
	Ui::AboutDialog ui;
};

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    im(std::make_unique<AboutDialog::Impl>(this))
{
	// No implementation.
}

AboutDialog::~AboutDialog()
{
	// No implementation.
}
