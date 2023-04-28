//
// Created by sr-71 on 19/04/2023.
//

#ifndef IRIDIUM_GENERALFRAME_HPP
#define IRIDIUM_GENERALFRAME_HPP

#include <QFrame>
#include <QSpinBox>
#include <QComboBox>

class GeneralFrame : public QFrame
{
Q_OBJECT

	QSpinBox *m_maxRcloneExecution{}, *m_parallelTransfers{}, *m_maxDepth{},*m_statsDuration{};
	QComboBox *m_loadType{};


public:
	explicit GeneralFrame(QWidget *parent = nullptr);

private:
	void connectSignals();

};


#endif //IRIDIUM_GENERALFRAME_HPP
