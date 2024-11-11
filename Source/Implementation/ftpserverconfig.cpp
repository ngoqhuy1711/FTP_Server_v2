//
// Created by ngoqh on 11/11/2024.
//

#include "ftpserverconfig.h"
#include <fstream>
#include <iostream>

FTPAccount::FTPAccount() {
}

FTPAccount::FTPAccount(string username): Account(username) {
}

FTPAccount::~FTPAccount() {
}

FTPServerConfig::FTPServerConfig() {
}

FTPServerConfig::~FTPServerConfig() {
}

string FTPAccount::getUserName() const {
    return this->username;
}

bool FTPServerConfig::loadAccountsFromFile(const string &fileName) {
    ifstream file(fileName, ios::in);
    if (!file.good()) {
        std::cerr << "Lỗi: Không thể lấy danh sách tài khoản." << std::endl;
        return false;
    } else {
        string stringLine;
        while (!file.eof()) {
            getline(file, stringLine);
            if (!stringLine.empty()) {
                FTPAccount *account = new FTPAccount();
                account->setUserName(stringLine);
                addAccount(account);
            }
        }
        file.close();
        return true;
    }
}
