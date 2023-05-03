//
// Created by Rahman on 16/04/2023.
//

#ifndef IRIDIUM_PROGRESSBAR_HPP
#define IRIDIUM_PROGRESSBAR_HPP

#include <QWidget>
#include <QTimer>

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

    double_t m_value{}, m_max_value{}, m_min_value{}, m_timer_counter{};
    Type m_type{};
    bool m_is_indeterminate{false}, m_show_progress{false};
    QTimer *m_timer{};
    State m_state{Progress};

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
        if (m_type == Circular)
            return {(QWidget::height() + QWidget::width()) / 2, (QWidget::height() + QWidget::width()) / 2};
        return {QWidget::width(), QWidget::height()};
    };
};


#endif //IRIDIUM_PROGRESSBAR_HPP
