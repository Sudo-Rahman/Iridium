//
// Created by sr-71 on 06/06/2023.
//

#ifndef IRIDIUM_CMDTEXTEDIT_HPP
#define IRIDIUM_CMDTEXTEDIT_HPP

#include <QTextEdit>
#include <QThread>
#include <QKeyEvent>
#include <QTimer>
#include <QProcess>


class CmdTextEdit : public QTextEdit
{
    Q_OBJECT

        QString _cmd;
    QProcess *_process{};

public:

    explicit CmdTextEdit(QWidget *parent = nullptr);

    ~CmdTextEdit() override
    {
        if (_process)
            _process->kill();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void hideEvent(QHideEvent *event) override;

    void showEvent(QShowEvent *event) override;

private:
    void createCmdProcess();

    void writeCmdInput();

    void cursorPosition();

    signals:

        void cmdOutputReady(QString);

};


#endif //IRIDIUM_CMDTEXTEDIT_HPP
