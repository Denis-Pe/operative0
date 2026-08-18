#ifndef OPERATIVE_ERROR_H
#define OPERATIVE_ERROR_H

void panicf(const char* format, ...);
void panic_errno(void);
void panic(void);

#define panic_switch() panicf("Error: switch statement is inexhaustive in %s %s\n", __FILE__, __LINE__)

#endif //OPERATIVE_ERROR_H
