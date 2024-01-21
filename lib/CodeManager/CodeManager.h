//
// Created by behemot on 27.12.2023.
//

#ifndef JUREKLOCK1_CODEMANAGER_H
#define JUREKLOCK1_CODEMANAGER_H

class CodeManager
{
private:
    CodeManager();

    const unsigned char* defaultCode;
    unsigned char code[4];

    void setCode(unsigned char[4]);
    void loadCode();
public:
    explicit CodeManager(const unsigned char defaultCode[4]);

    unsigned char* getCode();
    bool checkCode (unsigned char[4]);
    void saveCode(unsigned char*);
    void factoryReset();
};

#endif //JUREKLOCK1_CODEMANAGER_H
