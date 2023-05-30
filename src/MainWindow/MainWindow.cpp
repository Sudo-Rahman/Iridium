//
// Created by Rahman on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QTimer>
#include <Settings.hpp>
#include <Menu/MenuBar.hpp>
#include "SearchWidget.hpp"
#include <ExplorerWidget.hpp>
#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include <ProgressBar.hpp>

namespace bfs = boost::filesystem;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Iridium");
    setContentsMargins(0, 0, 0, 0);
    resize(Settings::getValue<int>(Settings::Width), Settings::getValue<int>(Settings::Height));
    setMinimumSize(1200, 600);
    QIcon::setThemeName("fluent");

    setMenuBar(new MenuBar(this));

    _tabWidget = new QTabWidget(this);
    setCentralWidget(_tabWidget);
    _tabWidget->setContentsMargins(0, 0, 0, 0);
    _tabWidget->addTab(new ExplorerWidget(this), tr("Explorateur"));
    _tabWidget->addTab(new SearchWidget(this), tr("Recherche"));


    _statusBar = new QStatusBar(this);
    setStatusBar(_statusBar);
    _statusBar->setLayoutDirection(Qt::RightToLeft);
    _statusBar->setContentsMargins(10, 0, 10, 0);

    connectSignals();

    Settings::refreshRemotesList();
}

void MainWindow::connectSignals()
{

    Rclone::rclone_not_exist.connect(
            [this]
            {
                if (_check_rclone)
                    return;
                auto btn = new QPushButton(tr("Télécharger Rclone !!!"));
                auto pallet = btn->palette();
                pallet.setColor(QPalette::ButtonText, Qt::red);
                btn->setPalette(pallet);
                btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                connect(btn, &QPushButton::clicked, this, &MainWindow::downloadRclone);
                _statusBar->addWidget(btn);
                _check_rclone = true;
            });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::setValue(Settings::Width, width(), Settings::Height, height());
    Settings::saveSettings();
    QMainWindow::closeEvent(event);
    RcloneManager::stopAll();
}

/**
 * @brief download rclone with curl
 */
void MainWindow::downloadRclone()
{
    _statusBar->removeWidget(_statusBar->findChild<QPushButton *>());
    auto dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(true);
    connect(dialog, &QDialog::finished, &Settings::refreshRemotesList);
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

    std::string os, arch;
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

    std::string url = "https://downloads.rclone.org/rclone-current-" + os + arch + ".zip";

    std::thread(
    [this, url, dialog]
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
        file = fopen("rclone.zip", "wb");
        if (file)
        {

            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            curl_easy_perform(curl);

            fclose(file);
        }
        curl_easy_cleanup(curl);

        curl_global_cleanup();

        // Unzip
        std::string command = "mkdir rcloneUnziped && tar -xf rclone.zip -C rcloneUnziped --strip-components 1";
        system(command.c_str());

        // Move rclone exe to current directory
        std::string rcloneName =
                Settings::getSystem().os not_eq Settings::Os::Windows ? "rclone" : "rclone.exe";
        bfs::copy_file("rcloneUnziped/" + rcloneName, rcloneName, bfs::copy_option::overwrite_if_exists);

        bfs::remove("rclone.zip");
        bfs::remove_all("rcloneUnziped");
        // execute by main thread
        dialog->close();
        _check_rclone = false;
    }).detach();
}