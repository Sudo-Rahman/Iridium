//
// Created by Rahman on 31/03/2023.
//

#include "TreeFileItemDistant.hpp"

TreeFileItemDistant::TreeFileItemDistant(const QString &path, const RemoteInfoPtr &remoteInfo, const boost::json::object &data,
                                         TreeFileItem *parent) : TreeFileItem(parent)
{
    try{
        m_file = std::make_shared<RcloneFile>(
                path + data.find("Name")->value().as_string().c_str(),
                static_cast<uint64_t>(data.find("Size")->value().as_int64()),
                data.find("IsDir")->value().as_bool(),
                QDateTime::fromString(data.find("ModTime")->value().as_string().c_str(), Qt::ISODateWithMs),
                remoteInfo);
        setText(m_file->getName());
        QStandardItem::setData(m_file->getName(), SORT_ROLE);
        initIcon();
        if(not m_file->isDir())
            setFlags(flags() & ~Qt::ItemIsDropEnabled);
    }catch(...){
        qDebug() << "Error while creating TreeFileItemDistant";}
}
