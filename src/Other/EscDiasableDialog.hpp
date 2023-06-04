//
// Created by sr-71 on 04/06/2023.
//

#ifndef IRIDIUM_ESCDIASABLEDIALOG_HPP
#define IRIDIUM_ESCDIASABLEDIALOG_HPP

#include <QDialog>
#include <QKeyEvent>

class EscDiasableDialog : public QDialog
{
Q_OBJECT

public:
    explicit EscDiasableDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowFlags(Qt::Dialog |Qt::FramelessWindowHint);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Escape)
            event->ignore();
        else
            QDialog::keyPressEvent(event);
    }
};


#endif //IRIDIUM_ESCDIASABLEDIALOG_HPP
