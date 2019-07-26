#ifndef RUC_SCANNER_H
#define RUC_SCANNER_H

void        onemore(ruc_context *context);
extern int  scan(ruc_context *context);
extern int  getnext(ruc_context *context);
extern int  scaner(ruc_context *context);
extern void nextch(ruc_context *context);
extern int  letter(ruc_context *);
extern int  digit(ruc_context *);
extern int  equal(ruc_context *, int, int);

#endif