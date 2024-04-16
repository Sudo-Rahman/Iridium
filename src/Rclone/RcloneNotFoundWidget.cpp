//
// Created by sr-71 on 15/04/2024.
//

#include "RcloneNotFoundWidget.hpp"

#include <QFileDialog>
#include <QVBoxLayout>
#include <Settings.hpp>
#include "MainWindow.hpp"

RcloneNotFoundWidget::RcloneNotFoundWidget(QWidget *parent) : QDialog(parent)
{
	this->setWindowTitle(tr("Rclone introuvable"));

    setFixedSize(300, 150);

	_rclone_path = new QLineEdit(this);
	_rclone_path->setEnabled(false);
	_ok_button = new QPushButton(tr("Ok"), this);
	_ok_button->setDefault(true);
	_ok_button->setEnabled(false);

	auto layout = new QVBoxLayout(this);
	_download = new QPushButton(tr("Télécharger rclone"), this);
	layout->addWidget(_download);
	layout->addWidget(_rclone_path);
	layout->addWidget(_ok_button);

	connect(_download, &QPushButton::clicked, this, [this]
	{
		this->downloadRclone();
	});

	connect(_ok_button, &QPushButton::clicked, this, [this] { this->close(); });
}

#include <curl/curl.h>
#include <zip.h>
#include <fstream>
#include <filesystem>
#include <boost/dll.hpp>
#include <ProgressBar.hpp>
#include <EscDiasableDialog.hpp>


/**
 * @brief download rclone with curl and unzip it with libzip
 */
void RcloneNotFoundWidget::downloadRclone()
{
    using namespace std;
    using namespace std::filesystem;

    // get iridium path
    auto pwd = boost::dll::program_location().parent_path().string() + "/";

    auto dialog = new EscDiasableDialog(this);
    dialog->setModal(true);
    connect(dialog, &QDialog::finished, this,[pwd,this]
    {
       _ok_button->setEnabled(true);
        _rclone_path->setText(pwd.c_str() + Settings::rcloneBaseName());
        _rclone_path->setToolTip(_rclone_path->text());
    });
    auto layout = new QVBoxLayout(dialog);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    auto label = new QLabel(tr("Téléchargement en cours..."), dialog);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    auto progressBar = new ProgressBar(ProgressBar::Circular, dialog);
    progressBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    progressBar->setFixedSize(100, 100);
    progressBar->setRange(0, 0);
    layout->addWidget(progressBar, 0, Qt::AlignCenter);
    dialog->open();

    string os, arch;
    switch (Settings::getSystem().os)
    {
        case Settings::Windows:
            os = "windows";
            break;
        case Settings::Linux:
            os = "linux";
            break;
        case Settings::MacOs:
            os = "osx";
            break;
        default:
            break;
    }
    switch (Settings::getSystem().arch)
    {
        case Settings::X86:
            arch = "-386";
            break;
        case Settings::x86_64:
            arch = "-amd64";
            break;
        case Settings::Arm64:
            arch = "-arm64";
            break;
        default:
            break;
    }

    string url = "https://downloads.rclone.org/rclone-current-" + os + arch + ".zip";

    _thread = boost::thread(
            [this,pwd, url, dialog]
            {
                CURL *curl = curl_easy_init();

                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 +[](void *ptr, size_t size, size_t nmemb, void *stream) -> size_t
                                 {
                                     size_t written = fwrite(ptr, size, nmemb, (FILE *) stream);
                                     return written;
                                 });

                FILE *file;
                file = fopen(string{pwd + "rclone.zip"}.c_str(), "wb");
                if (file)
                {
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

                    curl_easy_perform(curl);

                    fclose(file);
                }
                curl_easy_cleanup(curl);

                curl_global_cleanup();

                // Unzip
                int err = 0;
                auto *z = zip_open(string{pwd + "rclone.zip"}.c_str(), 0, &err);
                struct zip_stat sb{};
                auto file_name = string{string{"rclone"}.c_str()} +
                                 string{Settings::getSystem().os == Settings::Os::Windows ? ".exe" : ""};

                if (z == nullptr)
                    return;

                for (int i = 0; i < zip_get_num_entries(z, 0); i++)
                {
                    if (zip_stat_index(z, i, 0, &sb) == 0)
                    {
                        auto f = string{sb.name};
                        f = f.substr(f.find_last_of('/') + 1, f.size() - 1);
                        // check if file is rclone executable
                        if (f == file_name)
                        {
                            auto zf = zip_fopen_index(z, i, 0);
                            auto rclone_file = ofstream(pwd + file_name);
                            permissions(pwd + file_name, perms::all);

                            auto sum = 0;
                            char buf[100];
                            while (sum != sb.size)
                            {
                                auto len = zip_fread(zf, buf, 100);
                                if (len < 0)
                                {
                                    cerr << "error unzip" << endl;
                                    return;
                                }
                                rclone_file.write(buf, len);
                                sum += len;
                            }
                            rclone_file.close();
                            zip_fclose(zf);
                            break;
                        }
                    }
                }
                zip_close(z);

                ::remove(string{pwd + "rclone.zip"}.c_str());
                _downloaded = true;
                dialog->close();
            });
}