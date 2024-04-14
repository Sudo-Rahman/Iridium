//
// Created by Rahman on 11/01/2023.
//

#pragma once

#include <QGroupBox>
#include <iridium/entities/remote.hpp>
#include "RemoteConfigParamsFrame.hpp"

class RemoteWidgetParam : public QGroupBox
{
Q_OBJECT

private:
    ire::remote::remote_type _type;
    QIcon _icon{};
    bool _hover{}, _click{}, _selected{};
    RemoteConfigParamsFrame *_paramsFrame{};

    void initParamsFrame();

    void addBlur();

public:
    explicit RemoteWidgetParam(ire::remote::remote_type type, QWidget *parent = nullptr);

    [[nodiscard]] RemoteConfigParamsFrame *getParamsFrame() const;

    void select()
    {
        _selected = true;
        repaint();
    }

    void unselect()
    {
        _selected = false;
        repaint();
    }


protected:

    virtual bool event(QEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

signals:

    void clicked(RemoteConfigParamsFrame *);

    void newRemoteAdded();
};