#ifndef MYMACRO
#define MYMACRO

#define NEWLINE(ff) fscanf(ff,"%*[^\n]");fscanf(ff,"%*1c")
#define NEXTNUMB(ff) fscanf(ff,"%*[^-.0123456789]")
#define ENDFILE(ff) fseek(ff,0,SEEK_END)
#define BEGFILE(ff) fseek(ff,0,SEEK_SET)

#endif
