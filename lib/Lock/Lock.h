//
// Created by behemot on 27.12.2023.
//

#ifndef JUREKLOCK1_LOCK_H
#define JUREKLOCK1_LOCK_H

class Lock
{
private:
    Lock();
    unsigned char code[4];

public:
    explicit Lock(unsigned char defaultCode[4]);

    void setCode(unsigned char[4]);
    unsigned char* getCode();
    bool checkCode (unsigned char[4]);
};

#endif //JUREKLOCK1_LOCK_H
