#ifndef _GLOBAL_H
#define _GLOBAL_H 1

#include "defs.h"
#include "stdio.h"

struct {
  int first;
  int second;
} numr;

FILE input;
FILE output;
char source_file_path[MAXSTRINGL];

double numdouble;
int line = 0;
int mline = 0; 
int charnum = 1; 
int m_charnum = 1; 
int cur;
int next; 
int next1; 
int num; 
int hash; 
int repr; 
int keywordsnum; 
int wasstructdef = 0;

int source[SOURCESIZE];
int lines[LINESSIZE];
int before_source[SOURCESIZE];
int mlines[LINESSIZE];
int m_conect_lines[LINESSIZE];
int nextchar; 
int curchar;
int func_def;
int hashtab[256]; 
int reprtab[MAXREPRTAB]; 
int rp = 1;
int identab[MAXIDENTAB];
int id = 2;
int modetab[MAXMODETAB];
int md = 1;
int startmode = 1;

int stack[100];
int stackop[100];
int stackoperands[100];
int stacklog[100];
int sp = 0;
int sopnd = -1;
int aux = 0;
int lastid;
int curid = 2;
int lg = -1;
int displ = -3;
int maxdispl = 3;
int maxdisplg = 3;
int type;
int op = 0;
int inass = 0;
int firstdecl;

int iniprocs[INIPROSIZE];
int procd = 1;
int arrdim;
int arrelemlen;
int was_struct_with_arr;
int usual;

int instring = 0;
int inswitch = 0;
int inloop = 0;
int lexstr[MAXSTRINGL];
int tree[MAXTREESIZE];
int tc = 0;
int mtree[MAXTREESIZE];
int mtc = 0;
int mem[MAXMEMSIZE];
int pc = 4;
int functions[FUNCSIZE];
int funcnum = 2;
int functype;
int kw = 0;
int blockflag = 1;
int entry;
int wasmain = 0;
int wasret;
int wasdefault;
int notrobot = 1;
int prep_flag = 0;

int adcont;
int adbreak;
int adcase;
int adandor;
int switchreg;
int predef[FUNCSIZE];
int prdf = -1;
int emptyarrdef;

int gotost[1000];
int pgotost;

int _row = 1;
int _col = 0;

/*
 *  anst = VAL - значение на стеке
 *  anst = ADDR - на стеке адрес значения
 *  anst = NUMBER - ответ является константой
 *  anst = IDENT- значение в статике, в anstdisl смещение от l или g
 *  в ansttype всегда тип возвращаемого значения
 *  если значение указателя, адрес массива или строки лежит на верхушке стека, то это VAL, а не ADDR
 */
int anst;
int anstdispl;
int ansttype;
int leftansttype = -1;
int g;
int l;
int x;
int iniproc;

int bad_printf_placeholder = 0;


#endif
