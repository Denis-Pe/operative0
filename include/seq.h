//
// Created by denis on 7/12/26.
//

#ifndef OPERATIVE_SEQ_H
#define OPERATIVE_SEQ_H

#define DEFAULT_SEQ_SIZE 8

#include <stdlib.h>
#include <errno.h>
#include "error.h"

#define DECLARE_SEQ(SeqType, fun_spec, ElemType) typedef struct {\
    ElemType* ptr;\
    size_t len;\
    size_t cap;\
} SeqType;\
SeqType alloc_##fun_spec##_cap(const size_t cap);\
SeqType alloc_##fun_spec(void);\
void fun_spec##_push(SeqType *seq, const ElemType *elem);

#define DEFINE_SEQ(SeqType, fun_spec, ElemType)\
SeqType alloc_##fun_spec##_cap(const size_t cap) {\
    ElemType *ptr = malloc(sizeof(ElemType) * cap);\
    if (ptr == NULL) panic_errno();\
    const SeqType seq = {ptr, 0, cap};\
    return seq;\
}\
SeqType alloc_##fun_spec(void) {\
    return alloc_##fun_spec##_cap(DEFAULT_SEQ_SIZE);\
}\
void fun_spec##_push(SeqType *seq, const ElemType *elem) {\
    if (seq->len == seq->cap) {\
        size_t new_cap = seq->cap ? seq->cap * 2 : DEFAULT_SEQ_SIZE;\
        ElemType *new_ptr = realloc(seq->ptr, new_cap * sizeof(ElemType));\
        if (new_ptr == NULL) panic_errno();\
        seq->ptr = new_ptr;\
        seq->cap = new_cap;\
    }\
    seq->ptr[seq->len++] = *elem;\
}

#endif //OPERATIVE_SEQ_H
