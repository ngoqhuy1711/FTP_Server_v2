//
// Created by ngoqh on 11/11/2024.
//

#ifndef FTPSERVERCONFIG_H
#define FTPSERVERCONFIG_H
#include "serverconfig.h"


class FTPAccount : public Account {
private:
public:
    FTPAccount();

    FTPAccount(string userName);

    ~FTPAccount();

    string getUserName() const;
};

class FTPServerConfig : public ServerConfig {
private:
public:
    FTPServerConfig();

    ~FTPServerConfig();

    bool loadAccountsFromFile(const string &fileName) override;

};


#endif //FTPSERVERCONFIG_H
