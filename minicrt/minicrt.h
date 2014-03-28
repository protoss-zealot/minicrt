
#ifndef _MINI_CRT_H_
#define _MINI_CRT_H_


#ifdef __cplusplus
extern "C" {
#endif


#ifndef NULL
#define NULL (0)
#endif

void free(void *ptr);
void *malloc(unsigned size);
static int brk(void *end_data_segment);
char *strcpy(char *dest,const char *src);
unsigned strlen(const char *str);

/*Files and IOS*/
typedef int FILE;

#define EOF (-1)

#define stdin	((FILE *)0)
#define stdout	((FILE *)1)
#define stderr	((FILE *)2)

int mini_crt_init_io();
FILE *fopen(const char *filename,const char *mode);
int fread(void *buffer,int size,int count,FILE *stream);
int fwrite(const void *buffer,int size,int count,FILE *stream);
int fclose(FILE *fp);
int fseek(FILE *fp,int offset,int set);

/*for printf*/
int fputc(int c,FILE *stream);
int fputs(const char *str,FILE *stream);
int printf(const char *format,...);
int fprintf(FILE *stream,const char *format,...);

//internal
void do_global_crtors();
void mini_crt_call_exit_routine();

typedef void (*atexit_func_t)(void);
int atexit(atexit_func_t func);

#ifdef __cplusplus	
}
#endif


#endif	/*MINI CRT*/
