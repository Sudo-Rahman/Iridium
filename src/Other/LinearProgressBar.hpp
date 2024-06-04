#pragma once

#include <QWidget>
#include <QTimer>
#include <cmath>
#include <QPropertyAnimation>

class LinearProgressBar : public QWidget
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

    explicit LinearProgressBar(QWidget *parent = nullptr);

    int progress() const;
    void setProgress(double_t value);

    double_t minimum() const;
    double_t maximum() const;
    void setRange(double_t min, double_t max);

    State state() const;
    void setState(State state);

    void showPercentText(bool show);

    void error();

    void infinite();

    void reset();

    signals:
        void progressChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override { return QSize(200, 30); }

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
    double_t m_timer_counter{};
    bool m_show_percent{false};

    void updateAnimation() const;
    QColor currentColor() const;
};
