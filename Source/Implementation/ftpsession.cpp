//
// Created by ngoqh on 11/11/2024.
//
#include "ftpsession.h"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "ftpsession.h"
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
void FTPSession::doMKD(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doCWD(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doRMD(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doPORT(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doRETR(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doSTOR(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doQUIT(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doUnknown(string cmd_argv[], int cmd_argc) {

}

