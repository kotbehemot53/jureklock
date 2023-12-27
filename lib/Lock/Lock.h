//
// Created by behemot on 27.12.2023.
//

#ifndef JUREKLOCK1_LOCK_H
#define JUREKLOCK1_LOCK_H

class Lock
{
private:
    // TODO: define with constants?
    unsigned char code[4] = {0x45, 0x45, 0x45, 0x45};

public:
    void setCode(unsigned char[4]);
    unsigned char* getCode();
};

#endif //JUREKLOCK1_LOCK_H
