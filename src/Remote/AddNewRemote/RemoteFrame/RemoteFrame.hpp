//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_REMOTEFRAME_HPP
#define IRIDIUM_REMOTEFRAME_HPP

#include <QFrame>
#include "../../Remote.h"
#include "RemoteConfigParamsFrame.hpp"
#include "../../../Rclone/Rclone.hpp"

class RemoteFrame : public QFrame
{
Q_OBJECT
private:
    RemoteType type;
    QPixmap image{};
    bool clickRelease{};
    RemoteConfigParamsFrame *paramsFrame{};

    void initParamsFrame();

public:
    explicit RemoteFrame(RemoteType type, QWidget *parent = nullptr);

    [[nodiscard]] RemoteType getType() const;

    void setType(RemoteType type);

    [[nodiscard]] RemoteConfigParamsFrame *getParamsFrame() const;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

signals:

    void clicked(RemoteConfigParamsFrame *);
};


#endif //IRIDIUM_REMOTEFRAME_HPP
