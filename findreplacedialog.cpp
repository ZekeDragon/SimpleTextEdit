/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** findreplacedialog.cpp
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
#include "findreplacedialog.hpp"
#include "ui_findreplacedialog.h"

#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

struct FindReplaceDialog::Impl
{
	Impl(FindReplaceDialog *top) :
	    top(top)
	{
		ui.setupUi(top);
		focusFind(true);
		ui.notFoundLabel->setVisible(false);
		notFoundCooldown.setSingleShot(true);
		notFoundCooldown.setInterval(5000);
		QObject::connect(&notFoundCooldown, SIGNAL(timeout()), top, SLOT(silenceNotFound()));
	}

	void focusFind(bool findOrReplace)
	{
		focusedEdit = findOrReplace ? ui.findLineEdit : ui.replaceLineEdit;
	}

	void testReplaceButtons(QString const &newText)
	{
		ui.replaceButton   ->setDisabled(newText.isEmpty() || !ui.findNextButton->isEnabled());
		ui.replaceAllButton->setDisabled(newText.isEmpty() || !ui.findNextButton->isEnabled());
	}

	void testKeyChange(QKeyEvent *event)
	{
		if (event->modifiers() & Qt::ShiftModifier && event->modifiers() & Qt::ControlModifier)
		{
			if (!isBackwardsToggled)
			{
				isBackwardsToggled = true;
				ui.scanBackwardsCheck->toggle();
			}
		}
		else if (isBackwardsToggled)
		{
			isBackwardsToggled = false;
			ui.scanBackwardsCheck->toggle();
		}
	}

	FindFlags flags()
	{
		FindFlags flags = FFlags::None;
		if (ui.scanBackwardsCheck->isChecked()) flags |= FFlags::FindBackward;
		if (!ui.ignoreCaseCheck->isChecked()) flags |= FFlags::FindCaseSensitively;
		if (ui.matchRegexCheck->isChecked()) flags |= FFlags::FindByRegex;
		if (ui.wrapAroundCheck->isChecked()) flags |= FFlags::WrapAround;
		return flags;
	}

	FindReplaceDialog *top;
	Ui::FindReplaceDialog ui;
	QLineEdit *focusedEdit;
	QTimer notFoundCooldown;
	bool isBackwardsToggled = false;
};

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    im(std::make_unique<FindReplaceDialog::Impl>(this))
{
	// No implementation.
}

FindReplaceDialog::~FindReplaceDialog()
{
	// No implementation.
}

void FindReplaceDialog::focusFind(bool findOrReplace)
{
	im->focusFind(findOrReplace);
}

void FindReplaceDialog::findFieldChanged(QString const &newText)
{
	im->ui.findNextButton->setDisabled(newText.isEmpty());
	im->testReplaceButtons(im->ui.replaceLineEdit->text());
}

void FindReplaceDialog::replaceFieldChanged(QString const &newText)
{
	im->testReplaceButtons(newText);
}

void FindReplaceDialog::regexToggled(bool checked)
{
	im->ui.findLabel->setText(checked ? "Find Regex:" : "Find String:");
}

void FindReplaceDialog::findNextPressed()
{
	emit findRequested(im->flags(), im->ui.findLineEdit->text());
}

void FindReplaceDialog::replacePressed()
{
	emit replaceRequested(im->flags(), im->ui.findLineEdit->text(), im->ui.replaceLineEdit->text());
}

void FindReplaceDialog::replaceAllPressed()
{
	emit replaceAllRequested(im->flags(), im->ui.findLineEdit->text(), im->ui.replaceLineEdit->text());
}

void FindReplaceDialog::reportNoFind()
{
	im->ui.notFoundLabel->setVisible(true);
	im->notFoundCooldown.start();
}

void FindReplaceDialog::silenceNotFound()
{
	im->ui.notFoundLabel->setVisible(false);
}

void FindReplaceDialog::keyPressEvent(QKeyEvent *event)
{
	im->testKeyChange(event);
	QDialog::keyPressEvent(event);
}

void FindReplaceDialog::keyReleaseEvent(QKeyEvent *event)
{
	im->testKeyChange(event);
	QDialog::keyReleaseEvent(event);
}
