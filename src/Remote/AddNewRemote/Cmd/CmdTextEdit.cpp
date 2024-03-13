//
// Created by sr-71 on 06/06/2023.
//

#include "CmdTextEdit.hpp"
#include <IridiumApp.hpp>
#include <QTextBlock>
#include <QTextCursor>
#include <Settings.hpp>


CmdTextEdit::CmdTextEdit(QWidget * parent) : QTextEdit(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	connect(this, &CmdTextEdit::cmdOutputReady, this, &CmdTextEdit::append);

	_timer = new QTimer(this);
	_timer->setInterval(100);
	connect(_timer, &QTimer::timeout, this, [this]
	{
		for (const auto& line: _lines_tmp)
			emit cmdOutputReady(QString::fromStdString(line));
		emit cmdOutputReady("\n");
		_timer->stop();
		_lines_tmp.clear();
	});
	// mac os style scroll bar
	setStyleSheet("QTextEdit{background-color: #282c34; color: #abb2bf; border-radius: 25px; padding: 15px 10px;}");
	//                  "QScrollBar:vertical {border: none; background: transparent; width: 10px;}"
	//                  "QScrollBar::handle:vertical {background: #abb2bf; min-height: 20px; border-radius: 5px;}"
	//                  "QScrollBar::add-line:vertical {height: 0px; subcontrol-position: bottom; subcontrol-origin: margin;}"
	//                  "QScrollBar::sub-line:vertical { height: 0px; subcontrol-position: top; subcontrol-origin: margin;}"
	//                  "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {border: none; background: none;}");
}

void CmdTextEdit::cursorPosition()
{
	// move the cursor to the end of the line if the user tries to move it on up lines
	if (textCursor().blockNumber() < document()->blockCount() - 1)
		moveCursor(QTextCursor::End);

	// move the cursor to the end of the line if the user tries to move it on text that is not written by the user
	auto line = document()->findBlock(textCursor().position()).text();
	if (textCursor().positionInBlock() < line.length() - _cmd.length())
		moveCursor(QTextCursor::End);
}

/**
 * @brief create the process
 */
void CmdTextEdit::createCmdProcess()
{
	if (_process)
		delete _process;
	_process = new iridium::rclone::process;
	_process->config()
			.every_line([this](const std::string& line)
			{
				IridiumApp::runOnMainThread([this,line = std::move(line)]
				{
					_timer->stop();
					_lines_tmp.push_back(line);
					_timer->start();
				});
			})
			.execute();
}

/**
 * @brief write the command to the process
 */
void CmdTextEdit::writeCmdInput() { (*_process) << _cmd.toStdString() << iridium::rclone::process::endl; }


/**
 * @brief handle the key press event
 * @param event
 */
void CmdTextEdit::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return)
	{
		writeCmdInput();
		_cmd.clear();
	}
	else
		_cmd += event->text().toStdString();
	if (event->key() == Qt::Key_Backspace)
	{
		if (not _cmd.isEmpty())
		{
			cursorPosition();
			_cmd.removeLast();
		}
		else
			return;
	}
	cursorPosition();
	QTextEdit::keyPressEvent(event);
}

void CmdTextEdit::hideEvent(QHideEvent * event)
{
	if (_process->is_running())
		_process->stop();
	clear();
	Settings::refreshRemotesList();
	QTextEdit::hideEvent(event);
}

void CmdTextEdit::showEvent(QShowEvent * event)
{
	createCmdProcess();
	QTextEdit::showEvent(event);
}