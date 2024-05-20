//
// Created by Rahman on 16/04/2023.
//

#ifndef IRIDIUM_PROGRESSBAR_HPP
#define IRIDIUM_PROGRESSBAR_HPP

#include <QWidget>
#include <QTimer>
#include <Settings.hpp>

class ProgressBar : public QWidget
{
Q_OBJECT

public:
    enum State
    {
        Progress,
        Success,
        Error
    };
private:

    double_t _value{}, _max_value{}, _min_value{}, _timer_counter{};
    bool _is_indeterminate{false}, _show_progress{false};
    QTimer *_timer{};
    State _state{Progress};

    QColor _current{}, _progress{69, 164, 235}, _success{128, 211, 132}, _error{255, 118, 118}, _highlight{
            palette().color(QPalette::Highlight)};

public:
    explicit ProgressBar(QWidget *parent = nullptr);

    void setValue(double_t);

    void setMaxValue(double_t);

    void setMinValue(double_t);

    void setShowProgress(bool);

    void setIsIndeterminate(bool);

    void setRange(double_t min, double_t max);

    void error();

    void success();

protected:
    void paintEvent(QPaintEvent *event) override;

    // resiez if circular width = height
    [[nodiscard]] QSize sizeHint() const override
    {
        return QSize(100, 10);
    };
};


#endif //IRIDIUM_PROGRESSBAR_HPP
