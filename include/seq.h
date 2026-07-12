//
// Created by denis on 7/12/26.
//

#ifndef OPERATIVE_SEQ_H
#define OPERATIVE_SEQ_H

#define DEFAULT_SEQ_SIZE 8

#include <stdlib.h>

#define DEFINE_SEQ(SeqType, fun_spec, ElemType) typedef struct {\
    ElemType* ptr;\
    size_t len;\
    size_t cap;\
} SeqType;\
static inline SeqType alloc_##fun_spec##_cap(const size_t cap) {\
    ElemType *ptr = malloc(sizeof(ElemType) * cap);\
    const SeqType seq = {ptr, 0, cap};\
    return seq;\
}\
static inline SeqType alloc_##fun_spec() {\
    return alloc_##fun_spec##_cap(DEFAULT_SEQ_SIZE);\
}\
static inline  void fun_spec##_push(SeqType *seq, const ElemType *elem) {\
    if (seq->len == seq->cap) {\
        size_t new_cap = seq->cap? seq->cap*2 : DEFAULT_SEQ_SIZE;\
        seq->ptr = realloc(seq->ptr, new_cap * sizeof(ElemType));\
        seq->cap = new_cap;\
    }\
    seq->ptr[seq->len++] = *elem;\
}

#endif //OPERATIVE_SEQ_H
