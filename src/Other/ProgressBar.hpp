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
    enum Type
    {
        Circular,
        Linear
    };
    enum State
    {
        Progress,
        Success,
        Error
    };
private:

    double_t _value{}, _max_value{}, _min_value{}, _timer_counter{};
    Type _type{};
    bool _is_indeterminate{false}, _show_progress{false};
    QTimer *_timer{};
    State _state{Progress};

    QColor _current{}, _progress{69, 164, 235}, _success{128, 211, 132}, _error{255, 118, 118}, _highlight{
            palette().color(QPalette::Highlight)};

public:
    explicit ProgressBar(const Type &type, QWidget *parent = nullptr);

    void setValue(double_t);

    void setMaxValue(double_t);

    void setMinValue(double_t);

    void setType(Type mType);

    void setShowProgress(bool);

    void setIsIndeterminate(bool);

    void setRange(double_t min, double_t max);

    void error();

    void success();

protected:
    void paintEvent(QPaintEvent *event) override;

    void drawLinear(QPainter &painter);

    void drawCircular(QPainter &painter);

    // resiez if circular width = height
    [[nodiscard]] QSize sizeHint() const override
    {
        if (_type == Circular)
            return {(QWidget::height() + QWidget::width()) / 2, (QWidget::height() + QWidget::width()) / 2};
        return {QWidget::width(), QWidget::height()};
    };
};


#endif //IRIDIUM_PROGRESSBAR_HPP
