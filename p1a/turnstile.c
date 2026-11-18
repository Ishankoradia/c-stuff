#include <stdio.h>

enum State {LOCKED, UNLOCKED};
enum Input {COIN, PUSH};

enum State StateCalc(enum State current_state, enum Input inp) {
    if (current_state == LOCKED && inp == COIN) {
        return UNLOCKED;
    } else if (current_state == LOCKED && inp == PUSH) {
        return LOCKED;
    } else if (current_state == UNLOCKED && inp == PUSH) {
        return LOCKED;
    } else if (current_state == UNLOCKED && inp == COIN) {
        return UNLOCKED;
    } else {
        return current_state;
    }
}

int main(void) {
    enum State next = StateCalc(LOCKED, COIN);
    printf("The next state = %d\n", next);
    next = StateCalc(next, PUSH);
    printf("The next state = %d\n", next);
    next = StateCalc(next, COIN);
    printf("The next state = %d\n", next);
    next = StateCalc(next, PUSH);
    printf("The next state = %d\n", next);
}
