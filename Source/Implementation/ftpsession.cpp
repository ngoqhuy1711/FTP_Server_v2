//
// Created by ngoqh on 11/11/2024.
//
#include "ftpsession.h"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "ftpsession.h"
#include <direct.h>

#include <sstream>

#include "ftpserverconfig.h"


FTPSession::FTPSession(const TcpSocket &slave, ServerConfig *conf): Session(slave, conf) {
    this->sessionInfo = new FTPSessionInfo();
    this->quitSession = false;
    this->sessionInfo->status = STATUS_INIT;
}

void FTPSession::reset() {
    this->sessionInfo->status = STATUS_INIT;
    this->sessionInfo->username.clear();
    this->sessionInfo->password.clear();
}

FTPSession::~FTPSession() {
    delete sessionInfo;
}

void FTPSession::doUSER(string cmd_argv[], int cmd_argc) {
    if (cmd_argc < 2) {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string username = cmd_argv[1];
    if (conf->isValidUser(cmd_argv[1])) {
        sessionInfo->username = username;
        sessionInfo->status = STATUS_USER;
        response = "331 Mật khẩu cần thiết.\r\n";
        slave.send(response.c_str(), response.length());
    } else {
        response = "530 Tên người dùng không hợp lệ.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doPASS(string cmd_argv[], int cmd_argc) {
    if (cmd_argc < 2) {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status == STATUS_USER) {
        string password = cmd_argv[1];
        if (conf->authenticate(sessionInfo->username, password)) {
            sessionInfo->status = STATUS_PASS;
            response = "230 Đăng nhập thành công.\r\n";
            slave.send(response.c_str(), response.length());
        } else {
            response = "530 Mật khẩu không hợp lệ.\r\n";
            slave.send(response.c_str(), response.length());
        }
    }
}

void FTPSession::doMKD(string cmd_argv[], int cmd_argc) {
    if (cmd_argc < 2) {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS) {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string dir = cmd_argv[1];
    if (mkdir(dir.c_str()) == 0) {
        response = "257 Tạo thư mục thành công.\r\n";
        slave.send(response.c_str(), response.length());
    } else {
        response = "550 Tạo thư mục thất bại.\r\n";
        slave.send(response.c_str(), response.length());
    }

}

void FTPSession::doCWD(string cmd_argv[], int cmd_argc) {
}

void FTPSession::doRMD(string cmd_argv[], int cmd_argc) {
}

void FTPSession::doPORT(string cmd_argv[], int cmd_argc) {
    if (cmd_argc < 2) {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
    }
    if (sessionInfo->status != STATUS_PASS) {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
    }
    string portInfo = cmd_argv[1];
    std::replace(portInfo.begin(), portInfo.end(), ',', ' ');
    std::istringstream iss(portInfo);
    int h1, h2, h3, h4, p1, p2;
    iss >> h1 >> h2 >> h3 >> h4 >> p1 >> p2;
    string ip = std::to_string(h1) + "." + std::to_string(h2) + "." + std::to_string(h3) + "." + std::to_string(h4);
    unsigned short port = p1 * 256 + p2;
    sessionInfo->dataIpAddress = ip;
    sessionInfo->dataPort = port;
    response = "200 Mở cổng " + ip + ":" + std::to_string(port) + "thành công.\r\n";
    slave.send(response.c_str(), response.length());
}

void FTPSession::doRETR(string cmd_argv[], int cmd_argc) {
}

void FTPSession::doDELE(string cmd_argv[], int cmd_argc) {
}

void FTPSession::doQUIT(string cmd_argv[], int cmd_argc) {
    if (cmd_argc < 1) {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status == STATUS_PASS) {
        response = "211 Tạm biệt";
        slave.send(response.c_str(), response.length());
        quitSession = true;
    }
}

void FTPSession::doUnknown(string cmd_argv[], int cmd_argc) {
}
