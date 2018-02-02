#ifndef UTIL_H
#define UTIL_H

#include <windows.h>
#include <stdint.h>

#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

#define RELEASE_NON_NULL(x) \
{                           \
  if(x) x->Release();       \
  x = NULL;                 \
}

#define DELETE_AND_NULL(x)  \
{                           \
  delete x;                 \
  x = NULL;                 \
}

uint64_t genUUID(void);

// Relies on Logger already being initialized.
bool HR_FAILED(HRESULT hr);

#endif