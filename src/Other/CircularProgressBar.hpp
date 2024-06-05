// CircularProgressBar.h

#pragma once

#include <QWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <cmath>

class CircularProgressBar : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
	enum State {
		Progress,
		Success,
		Error,
	};
	Q_ENUM(State)

	explicit CircularProgressBar(QWidget *parent = nullptr);

	int progress() const;
	void setProgress(double_t value);

	int minimum() const;
	int maximum() const;
	void setRange(double_t min, double_t max);

	State state() const;
	void setState(State state);

	void setSize(const int &size);

	void showPercentText(bool show);

	void infinite();

	void reset();

	signals:
		void progressChanged();

protected:
	void paintEvent(QPaintEvent *event) override;
	QSize sizeHint() const override { return QSize(100, 100); }

private:
	double_t m_progress;
	double_t m_min;
	double_t m_max;
	State m_state;
    QTimer *m_timer;
	QColor m_accentColor;
	QColor m_progressColor;
	QColor m_successColor;
	QColor m_errorColor;
	uint64_t m_timer_counter{};
	bool m_show_percent{true};

	void updateAnimation() const;
	QColor currentColor() const;

	void setBaseSize(const QSize &);
	void setBaseSize(int basew, int baseh);

	void setFixedSize(const QSize &);
	void setFixedSize(int w, int h);
	void setFixedWidth(int w);
	void setFixedHeight(int h);

};