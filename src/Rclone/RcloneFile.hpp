#pragma once



#include <QDir>
#include <QIcon>
#include <boost/json.hpp>
#include <Remote.h>
#include <QMimeType>
#include <iridium/entities.hpp>

class RcloneFile : public ire::file
{
public:

    RcloneFile(file * parent, const QString &file_name, int64_t size, bool is_dir,
               const QDateTime &mod_time, const RemoteInfoPtr &remote = nullptr);

	explicit RcloneFile(const ire::file &file);

	RcloneFile() = default;


    [[nodiscard]] const QString getPath() const;

    void setName(const QString &path);

    [[nodiscard]] uint64_t getSize() const;

    void setSize(uint64_t size);

    [[nodiscard]] bool isDir() const;

    void setIsDir(bool isDir);

    [[nodiscard]] const QDateTime getModTime() const;

    void setModTime(const QDateTime &modTime);

    [[nodiscard]] QString getName() const;

    [[nodiscard]] QString getSizeString() const;

    [[nodiscard]] QString getPathString() const;

    [[nodiscard]] QString getModTimeString() const;

    [[nodiscard]] RemoteInfoPtr getRemoteInfo() const;

    [[nodiscard]] uint32_t getObjs() const;

    [[nodiscard]] QString getFileType() const;

    [[nodiscard]] QIcon getIcon() const;

    [[nodiscard]]  QList<QMimeType> mimeTypes() const;
};

typedef std::shared_ptr<RcloneFile> RcloneFilePtr;
