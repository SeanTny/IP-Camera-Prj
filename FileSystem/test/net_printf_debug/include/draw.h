
#ifndef _DRAW_H
#define _DRAW_H
int OpenTextFile(char *pcFileName);
int SetTextDetail(char *pcHZKFile, char *pcFileFreetype, unsigned int dwFontSize);
int SelectAndInitDisplay(char *pcName);
int ShowNextPage(void);
int ShowPrePage(void);
int GetDispResolution(int *piXres, int *piYres);

#endif /* _DRAW_H */

