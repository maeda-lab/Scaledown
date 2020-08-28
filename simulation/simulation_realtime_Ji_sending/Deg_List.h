// Masahiro Furukawa
// Aug 28, 2020

#pragma once

#include <stdio.h>

//=================================================angle table=======================================================
#define _NUM_DEG_SEQ_ 3000
class Deg_List {

private:
    struct deg {
        double j1;
        double j2;
        double j3;
    };
    deg deg_seq[_NUM_DEG_SEQ_];
    int ptr;
public:
    Deg_List(void) {
        init();
    }
    void init(void) {
        ptr = 0;
        for (int i = 0; i < _NUM_DEG_SEQ_; i++) {
            deg_seq[i].j1 = 0.0;
            deg_seq[i].j2 = 0.0;
            deg_seq[i].j3 = 0.0;
        }
    }
    void setStart(void) {
        ptr = 0;
    }
    int add(double J1, double J2, double J3) {
        if (ptr < _NUM_DEG_SEQ_) {
            deg_seq[ptr].j1 = J1;
            deg_seq[ptr].j2 = J2;
            deg_seq[ptr].j3 = J3;
            return (ptr++);
        }
        else { return -1; }
    }
    int getNext(double* J1, double* J2, double* J3) {
        if (ptr < _NUM_DEG_SEQ_) {
            memcpy(J1, &deg_seq[ptr].j1, sizeof(double));
            memcpy(J2, &deg_seq[ptr].j2, sizeof(double));
            memcpy(J3, &deg_seq[ptr].j3, sizeof(double));
            return (ptr++);
        }
        else { return -1; }
    }
    int getLen(void) {
        return ptr;
    }
};
