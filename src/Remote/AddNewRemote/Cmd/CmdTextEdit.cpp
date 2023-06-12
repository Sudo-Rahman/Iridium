//
// Created by sr-71 on 06/06/2023.
//

#include "CmdTextEdit.hpp"
#include <Global.hpp>
#include <QTextBlock>
#include <QTextCursor>
#include <Settings.hpp>


CmdTextEdit::CmdTextEdit(QWidget *parent) : QTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    connect(this, &CmdTextEdit::cmdOutputReady, this, &CmdTextEdit::append);

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
    _process = new QProcess(this);
    connect(_process, &QProcess::readyReadStandardOutput, [this]
    {
        emit cmdOutputReady(QString::fromUtf8(_process->readAllStandardOutput()));
    });
    _process->setProgram(Iridium::Global::path_rclone.c_str());
    _process->setArguments({"config"});
    _process->setProcessChannelMode(QProcess::MergedChannels);
    _process->start();
}

/**
 * @brief write the command to the process
 */
void CmdTextEdit::writeCmdInput()
{
    _process->write((_cmd + "\n").toLocal8Bit());
}


/**
 * @brief handle the key press event
 * @param event
 */
void CmdTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        writeCmdInput();
        _cmd.clear();
    } else
        _cmd += event->text().toStdString();
    if (event->key() == Qt::Key_Backspace)
    {
        if (not _cmd.isEmpty())
        {
            cursorPosition();
            _cmd.removeLast();
        } else
            return;
    }
    cursorPosition();
    QTextEdit::keyPressEvent(event);
}

void CmdTextEdit::hideEvent(QHideEvent *event)
{
    if (_process)
        _process->kill();
    clear();
    Settings::refreshRemotesList();
    QTextEdit::hideEvent(event);
}

void CmdTextEdit::showEvent(QShowEvent *event)
{
    createCmdProcess();
    QTextEdit::showEvent(event);
}