#ifndef OPERATIVE_ERROR_H
#define OPERATIVE_ERROR_H

void panicf(const char* format, ...);
void panic_errno(void);
void panic(void);

#endif //OPERATIVE_ERROR_H
