//
// Created by behemot on 27.12.2023.
//

#ifndef JUREKLOCK1_LOCK_H
#define JUREKLOCK1_LOCK_H

#define LOCK_DEFAULT_DIGIT_0 0x45
#define LOCK_DEFAULT_DIGIT_1 0x45
#define LOCK_DEFAULT_DIGIT_2 0x45
#define LOCK_DEFAULT_DIGIT_3 0x45

class Lock
{
private:
    unsigned char code[4] = {LOCK_DEFAULT_DIGIT_0, LOCK_DEFAULT_DIGIT_1, LOCK_DEFAULT_DIGIT_2, LOCK_DEFAULT_DIGIT_3};

public:
    void setCode(unsigned char[4]);
    unsigned char* getCode();
    bool checkCode (unsigned char[4]);
};

#endif //JUREKLOCK1_LOCK_H
