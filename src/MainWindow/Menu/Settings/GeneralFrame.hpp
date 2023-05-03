//
// Created by Rahman on 02/05/2023.
//

#ifndef IRIDIUM_GENERALFRAME_HPP
#define IRIDIUM_GENERALFRAME_HPP

#include <QFrame>
#include <QSpinBox>
#include <QComboBox>

class GeneralFrame : public QFrame
{
Q_OBJECT

    QComboBox *m_language{};

public:
    explicit GeneralFrame(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_GENERALFRAME_HPP
