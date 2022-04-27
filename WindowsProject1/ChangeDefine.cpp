#include "framework.h"

void ChangeMAXBYTES(int num) {
    switch (num) {
    case 8:
#define MAX_BYTES 10
        break;
    case 10:
        num = 8;
        break;
    default:
        break;

    }

    return;
}