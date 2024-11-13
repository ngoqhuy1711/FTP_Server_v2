//
// Created by ngoqh on 11/11/2024.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "ftpserver.h"

const char FTP_CONF_FILE_NAME[] = "ftp.txt";
const char FTP_ACCOUNT_FILE_NAME[] = "ftpaccount.txt";

const char FTP_DELIMITER[] = " ";
const char EOL[] = "'\r\n";
const char BLANK_LINE[] = "\r\n";

const char FTP_GREETING[] = "220 FTP Server v2.0 đã sẵn sàng.\r\n";

FTPServer::FTPServer(unsigned short port): TCPServer(port) {
    initCmd();
    conf = new FTPServerConfig();
}

bool FTPServer::loadServerConfig(const string &confFileName) {
    ifstream file(confFileName, ios::in);
    if (!file.good()) {
        std::cerr << "Lỗi: Không thể mở file cấu hình." << std::endl;
        return false;
    } else {
        string stringLine;
        string key, value;
        while (!file.eof()) {
            getline(file, stringLine);
            if (readAttribute(stringLine, key, value)) {
                if (key == "port") {
                    port = stoi(value);
                } else if (key == "conn-timeout") {
                    int connTimeout = stoi(value);
                    getServerConfig()->setTimeOut(connTimeout);
                }
            }
        }
        file.close();
        return true;
    }
}

bool FTPServer::configServer() {
    return (loadServerConfig(FTP_CONF_FILE_NAME) && conf->loadAccountsFromFile(FTP_ACCOUNT_FILE_NAME));
}

void FTPServer::cleanServer() {
    conf->removeAllAccount();
}

FTPServer::~FTPServer() {
    if (isRunning()) {
        stop();
        std::cout << "Server đã dừng.\n" << std::endl;
    }
}

void FTPServer::initCmd() {
    addCmd(USER, FUNC_CAST(&FTPSession::doUSER));
    addCmd(PASS, FUNC_CAST(&FTPSession::doPASS));
    addCmd(MKD, FUNC_CAST(&FTPSession::doMKD));
    addCmd(CWD, FUNC_CAST(&FTPSession::doCWD));
    addCmd(RMD, FUNC_CAST(&FTPSession::doRMD));
    addCmd(PORT, FUNC_CAST(&FTPSession::doPORT));
    addCmd(RETR, FUNC_CAST(&FTPSession::doRETR));
    addCmd(STOR, FUNC_CAST(&FTPSession::doSTOR));
    addCmd(QUIT, FUNC_CAST(&FTPSession::doQUIT));
}

int FTPServer::readCmd(TcpSocket &slave, string &cmdLine) {
    try {
        char cmdBuffer[256];
        int byteRead = slave.recvLine(cmdBuffer, sizeof(cmdBuffer));
        if (byteRead >= 2) {
            cmdBuffer[byteRead - 2] = 0;
            cmdLine = cmdBuffer;
        }
        return byteRead;
    } catch (SocketException &e) {
        std::cerr << "Lỗi: " << e.what() << std::endl;
        return -1;
    }
}

unsigned short FTPServer::parseCmd(const string &cmdLine, string cmd_argv[], int &cmd_argc) {
    cmd_argc = 0;
    cmd_argv[cmd_argc].clear();
    istringstream iss(cmdLine);
    while (cmd_argc < SERVER_CMD_ARG_NUM && iss.good()) {
        iss >> cmd_argv[cmd_argc];
        cmd_argc++;
    }
    if (cmd_argc > 0 && !cmd_argv[0].empty()) {
        for (int i = 0; i < this->numCmd; i++) {
            if (cmd_argv[0] == cmdNameList[i]) {
                return i;
            }
        }
    }
    return SERVER_CMD_UNKNOWN;
}

void FTPServer::startNewSession(TcpSocket slave) {
    FTPSession *session = new FTPSession(slave, conf);
    string cmdLine;
    string cmdArgv[SERVER_CMD_ARG_NUM];
    int cmdArgc;
    unsigned short cmdId;
    int cmdLen;
    try {
        slave.send(FTP_GREETING, strlen(FTP_GREETING));
        while (!session->isQuit()) {
            cmdLen = readCmd(slave, cmdLine);
            if (cmdLen < 0) {
                break;
            }
            cmdId = parseCmd(cmdLine, cmdArgv, cmdArgc);
            doCmd(session, cmdId, cmdArgv, cmdArgc);
        }
        delete session;
    } catch (SocketException &e) {
        std::cerr << "Lỗi: " << e.what() << std::endl;
        delete session;
    }
}
