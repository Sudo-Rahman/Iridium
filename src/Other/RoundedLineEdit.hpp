//
// Created by sr-71 on 08/04/2023.
//

#ifndef IRIDIUM_ROUNDEDLINEEDIT_HPP
#define IRIDIUM_ROUNDEDLINEEDIT_HPP

#include <QLineEdit>
#include <QTimer>

class RoundedLineEdit : public QLineEdit
{
Q_OBJECT

	QTimer *m_timer;
	bool m_addCusrsor{};

public:
	explicit RoundedLineEdit(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent *event) override;

	bool event(QEvent *) override;

private:
	void addBlur();

};


#endif //IRIDIUM_ROUNDEDLINEEDIT_HPP
