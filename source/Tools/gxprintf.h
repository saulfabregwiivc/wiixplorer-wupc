#ifndef GXPRINTF_H_
#define GXPRINTF_H_

#ifdef __cplusplus
extern "C"{
#endif

void gxprintf(const char * format, ...);
void SetGXConsole(void * console);

#ifdef __cplusplus
}
#endif

#endif
