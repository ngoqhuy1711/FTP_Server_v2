//
// Created by ngoqh on 11/11/2024.
//
#include <iostream>
#include "ftpservercli.h"

FTPServerCLI::FTPServerCLI(): CmdLineInterface("Server>") {
    std::cout << "FTP Server v2.0" << std::endl;
    initCmd();
    server = new FTPServer();
    if (!server->configServer()) {
        std::cerr << "Không thể cấu hình server." << std::endl;
        delete server;
        exit(-1);
    }
}
FTPServerCLI::~FTPServerCLI() {
    delete server;
}
void FTPServerCLI::initCmd() {
    addCmd("start", CLI_CAST(&FTPServerCLI::doStart));
    addCmd("stop", CLI_CAST(&FTPServerCLI::doStop));
    addCmd("status", CLI_CAST(&FTPServerCLI::doStatus));
    addCmd("restart", CLI_CAST(&FTPServerCLI::doRestart));
    addCmd("help", CLI_CAST(&FTPServerCLI::doHelp));
}
void FTPServerCLI::doStart(string cmd_argv[], int cmd_argc) {
    if (server->isRunning()) {
        std::cout << "Server đang chạy.\n" << std::endl;
    } else if (server->start()) {
        std::cout << "Server đã bắt đầu.\n" << std::endl;
    } else {
        std::cout << "Không thể bắt đầu server.\n" << std::endl;
    }
}
void FTPServerCLI::doStop(string cmd_argv[], int cmd_argc) {
    server->stop();
    std::cout << "Server đã dừng.\n" << std::endl;
}
void FTPServerCLI::doStatus(string cmd_argv[], int cmd_argc) {
    if (server->isRunning()) {
        std::cout << "Server đang chạy.\n" << std::endl;
    } else {
        std::cout << "Server không chạy.\n" << std::endl;
    }
}
void FTPServerCLI::doRestart(string cmd_argv[], int cmd_argc) {
    if (server->restart()) {
        std::cout << "Server đã khởi động lại.\n" << std::endl;
    } else {
        std::cout << "Không thể khởi động lại server.\n" << std::endl;
    }
}
void FTPServerCLI::doHelp(string cmd_argv[], int cmd_argc) {
    std::cout << "Các lệnh hỗ trợ:\n";
    std::cout << "start: Bắt đầu server.\n";
    std::cout << "stop: Dừng server.\n";
    std::cout << "status: Trạng thái server.\n";
    std::cout << "restart: Khởi động lại server.\n";
    std::cout << "help: Hiển thị trợ giúp.\n";
}




