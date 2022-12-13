#ifndef H_CODEGEN
#define H_CODEGEN 1

void codegen();

#include "errors.h"
#include "extdecl.h"
#include "global.h"


int curth = 0;


void Declid_gen();


void tocode(int c)
{
  // printf("tocode tc=%i pc %i) %i\n", tc, pc, c);
  mem[pc++] = c;
}

void adbreakend()
{
  while (adbreak)
  {
    int r = mem[adbreak];
    mem[adbreak] = pc;
    adbreak = r;
  }
}

void adcontbeg(int ad)
{
  while (adcont != ad)
  {
    int r = mem[adcont];
    mem[adcont] = ad;
    adcont = r;
  }
}

void adcontend()
{
  while (adcont != 0)
  {
    int r = mem[adcont];
    mem[adcont] = pc;
    adcont = r;
  }
}

void finalop()
{
  int c;
  while ((c = tree[tc]) > 9000)
  {
    tc++;
    if (c != NOP)
    {
      if (c == ADLOGOR)
      {
        tocode(_DOUBLE);
        tocode(BNE0);
        tree[tree[tc++]] = pc++;
      }
      else if (c == ADLOGAND)
      {
        tocode(_DOUBLE);
        tocode(BE0);
        tree[tree[tc++]] = pc++;
      }
      else
      {
        tocode(c);
        if (c == LOGOR || c == LOGAND)
        {
          mem[tree[tc++]] = pc;
        }
        else if (c == COPY00 || c == COPYST)
        {
          tocode(tree[tc++]);	// d1
          tocode(tree[tc++]);	// d2
          tocode(tree[tc++]);	// длина
        }
        else if (c == COPY01 || c == COPY10 || c == COPY0ST || c == COPY0STASS)
        {
          tocode(tree[tc++]);	// d1
          tocode(tree[tc++]);	// длина
        }
        else if (c == COPY11 || c == COPY1ST || c == COPY1STASS)
        {
          tocode(tree[tc++]);	// длина
        }
        else if ((c >= REMASS && c <= DIVASS) || (c >= REMASSV && c <= DIVASSV) ||
            (c >= ASSR && c <= DIVASSR) || (c >= ASSRV && c <= DIVASSRV) || (c >= POSTINC && c <= DEC) ||
            (c >= POSTINCV && c <= DECV) || (c >= POSTINCR && c <= DECR) || (c >= POSTINCRV && c <= DECRV))
        {
          tocode(tree[tc++]);
        }
      }
    }
  }
}

int Expr_gen(int incond)
{
  int flagprim = 1;
  int eltype;
  int wasstring = 0;

  while (flagprim)
  {
    switch (tree[tc++])
    {
      case TIdent:
        anstdispl = tree[tc++];
        break;
      case TIdenttoaddr:
      {
        tocode(LA);
        tocode(anstdispl = tree[tc++]);
      }
        break;
      case TIdenttoval:
      {
        tocode(LOAD);
        tocode(tree[tc++]);
      }
        break;
      case TIdenttovald:
      {
        tocode(LOADD);
        tocode(tree[tc++]);
      }
        break;
      case TAddrtoval:
        tocode(LAT);
        break;
      case TAddrtovald:
        tocode(LATD);
        break;
      case TConst:
      {
        tocode(LI);
        tocode(tree[tc++]);
      }
        break;
      case TConstd:
      {
        tocode(LID);
        tocode(tree[tc++]);
        tocode(tree[tc++]);
      }
        break;
      case TString:
      {
        int n = tree[tc++];
        int res;
        int i;

        tocode(LI);
        tocode(res = pc + 4);
        tocode(B);

        pc += 2;
        for (i = 0; i < n; i++)
        {
          tocode(tree[tc++]);
        }

        mem[res - 1] = n;
        mem[res - 2] = pc;
        wasstring = 1;
      }
        break;
      case TDeclid:
        Declid_gen();
        break;
      case TBeginit:
      {
        int n = tree[tc++];
        int i;

        tocode(BEGINIT);
        tocode(n);

        for (i = 0; i < n; i++)
        {
          Expr_gen(0);
        }
      }
        break;
      case TStructinit:
      {
        int n = tree[tc++];
        int i;
        for (i = 0; i < n; i++)
        {
          Expr_gen(0);
        }
      }
        break;
      case TSliceident:
      {
        int sz = szof(eltype);
        tocode(LOAD);		// параметры - смещение идента и тип элемента
        tocode(tree[tc++]);	// продолжение в след case

        eltype = tree[tc++];

        Expr_gen(0);

        tocode(SLICE);
        tocode(sz);
        //tocode(szof(eltype));

        if (eltype > 0 && modetab[eltype] == MARRAY)
        {
          tocode(LAT);
        }
      }
        break;
      case TSlice:			// параметр - тип элемента
      {
        int sz = szof(eltype);
        eltype = tree[tc++];
        Expr_gen(0);

        tocode(SLICE);
        tocode(sz);
        //tocode(szof(eltype));

        if (eltype > 0 && modetab[eltype] == MARRAY)
        {
          tocode(LAT);
        }
      }
        break;
      case TSelect:
      {
        tocode(SELECT);		// SELECT field_displ
        tocode(tree[tc++]);
      }
        break;
      case TPrint:
      {
        tocode(_PRINT);
        tocode(tree[tc++]);	// type
      }
        break;
      case TCall1:
      {
        int i;
        int n = tree[tc++];

        tocode(CALL1);

        for (i = 0; i < n; i++)
        {
          Expr_gen(0);
        }
      }
        break;
      case TCall2:
      {
        tocode(CALL2);
        tocode(identab[tree[tc++] + 3]);
      }
        break;

      default:
        tc--;
    }

    finalop();

    if (tree[tc] == TCondexpr)
    {
      if (incond)
      {
        return wasstring;
      }
      else
      {
        int adelse;
        int ad = 0;
        do
        {
          tc++;
          tocode(BE0);
          adelse = pc++;
          Expr_gen(0);	// then
          tocode(B);
          mem[pc] = ad;
          ad = pc;
          mem[adelse] = ++pc;
          Expr_gen(1);	// else или cond
        } while (tree[tc] == TCondexpr);

        while (ad)
        {
          int r = mem[ad];
          mem[ad] = pc;
          ad = r;
        }
      }

      finalop();
    }

    if (tree[tc] == TExprend)
    {
      tc++;
      flagprim = 0;
    }
  }

  return wasstring;
}

void compstmt_gen();

void Stmt_gen()
{
  switch (tree[tc++])
  {
    case NOP:
      break;

    case CREATEDIRECTC:
      tocode(CREATEDIRECTC);
      break;

    case EXITC:
      tocode(EXITC);
      break;

    case TStructbeg:
    {
      tocode(B);
      tocode(0);
      iniprocs[tree[tc++]] = pc;
    }
      break;

    case TStructend:
    {
      int numproc = tree[tree[tc++] + 1];
      tocode(STOP);
      mem[iniprocs[numproc] - 1] = pc;
    }
      break;

    case TBegin:
      compstmt_gen();
      break;

    case TIf:
    {
      int elseref = tree[tc++];
      int ad;

      Expr_gen(0);
      tocode(BE0);
      ad = pc++;
      Stmt_gen();

      if (elseref)
      {
        mem[ad] = pc + 2;
        tocode(B);
        ad = pc++;
        Stmt_gen();
      }

      mem[ad] = pc;
    }
      break;
    case TWhile:
    {
      int oldbreak = adbreak;
      int oldcont = adcont;
      int ad = pc;

      adcont = ad;
      Expr_gen(0);
      tocode(BE0);
      mem[pc] = 0;
      adbreak = pc++;
      Stmt_gen();
      adcontbeg(ad);
      tocode(B);
      tocode(ad);
      adbreakend();
      adbreak = oldbreak;
      adcont = oldcont;
    }
      break;
    case TDo:
    {
      int oldbreak = adbreak;
      int oldcont = adcont;
      int ad = pc;

      adcont = adbreak = 0;
      Stmt_gen();
      adcontend();
      Expr_gen(0);
      tocode(BNE0);
      tocode(ad);
      adbreakend();
      adbreak = oldbreak;
      adcont = oldcont;
    }
      break;
    case TFor:
    {
      int fromref = tree[tc++];
      int condref = tree[tc++];
      int incrref = tree[tc++];
      int stmtref = tree[tc++];
      int oldbreak = adbreak;
      int oldcont = adcont;
      int incrtc;
      int endtc;
      int initad;

      if (fromref)
      {
        Expr_gen(0);	// init
      }

      initad = pc;
      adcont = adbreak = 0;

      if (condref)
      {
        Expr_gen(0);	// cond
        tocode(BE0);
        mem[pc] = 0;
        adbreak = pc++;
      }

      incrtc = tc;
      tc = stmtref;
      Stmt_gen();
      adcontend();

      if (incrref)
      {
        endtc = tc;
        tc = incrtc;
        Expr_gen(0);	// incr
        tc = endtc;
      }

      tocode(B);
      tocode(initad);
      adbreakend();
      adbreak = oldbreak;
      adcont = oldcont;
    }
      break;
    case TGoto:
    {
      int id1 = tree[tc++];
      int a;
      int _id;

      if (_id > 0)
        _id = id1;
      else
        _id = -id1;
      //int _id = id1 > 0 ? id1 : -id1;

      tocode(B);

      if ((a = identab[_id + 3]) > 0)	// метка уже описана
      {
        tocode(a);
      }
      else							// метка еще не описана
      {
        identab[_id + 3] = -pc;
        if (id1 < 0)
          tocode(0);	// первый раз встретился переход на еще не описанную метку или нет
        else
          tocode(a);
        // tocode(id1 < 0 ? 0 : a);  // первый раз встретился переход на еще не описанную метку или нет
      }
    }
      break;
    case TLabel:
    {
      int _id = tree[tc++];
      int a;

      if ((a = identab[_id + 3]) < 0)	// были переходы на метку
      {
        while (a)					// проставить ссылку на метку во всех ранних переходах
        {
          int r = mem[-a];
          mem[-a] = pc;
          a = r;
        }
      }
      identab[_id + 3] = pc;
    }
      break;
    case TSwitch:
    {
      int oldbreak = adbreak;
      int oldcase = adcase;

      adbreak = 0;
      adcase = 0;
      Expr_gen(0);
      Stmt_gen();

      if (adcase > 0)
      {
        mem[adcase] = pc;
      }

      adcase = oldcase;
      adbreakend();
      adbreak = oldbreak;
    }
      break;
    case TCase:
    {
      if (adcase)
      {
        mem[adcase] = pc;
      }

      tocode(_DOUBLE);
      Expr_gen(0);
      tocode(EQEQ);
      tocode(BE0);
      adcase = pc++;
      Stmt_gen();
    }
      break;
    case TDefault:
    {
      if (adcase)
      {
        mem[adcase] = pc;
      }

      adcase = 0;
      Stmt_gen();
    }
      break;

    case TBreak:
    {
      tocode(B);
      mem[pc] = adbreak;
      adbreak = pc++;
    }
      break;
    case TContinue:
    {
      tocode(B);
      mem[pc] = adcont;
      adcont = pc++;
    }
      break;
    case TReturnvoid:
    {
      tocode(RETURNVOID);
    }
      break;
    case TReturnval:
    {
      int d = tree[tc++];

      Expr_gen(0);
      tocode(RETURNVAL);
      tocode(d);
    }
      break;
    case TPrintid:
    {
      tocode(_PRINTID);
      tocode(tree[tc++]);	// ссылка в identtab
    }
      break;
    case TPrintf:
    {
      tocode(_PRINTF);
      tocode(tree[tc++]);	// общий размер того, что надо вывести
    }
      break;
    case TFprintf:
    {
      tocode(_FPRINTF);
      tocode(tree[tc++]); // общий размер того, что надо вывести
    }
      break;
    case TGetid:
    {
      tocode(_GETID);
      tocode(tree[tc++]);	// ссылка в identtab
    }
      break;

    default:
    {
      tc--;
      Expr_gen(0);
    }
      break;
  }
}

void Struct_init_gen()
{
  int i;
  int n;

  if (tree[tc] == TStructinit)
  {
    tc++;
    n = tree[tc++];

    for (i = 0; i < n; i++)
    {
      Struct_init_gen();
    }
    tc++;	// TExprend
  }
  else
  {
    Expr_gen(0);
  }
}

void Declid_gen()
{
  int olddispl = tree[tc++];
  int telem = tree[tc++];
  int N = tree[tc++];
  int element_len;
  int all = tree[tc++];		// all - общее кол-во слов в структуре
                // all == 0 нет инициализатора,
                // all == 1 есть инициализатор,
                // all == 2 есть инициализатор только из строк
  int iniproc = tree[tc++];
  int usual = tree[tc++];		// для массивов есть еще usual
                // == 0 с пустыми границами,
                // == 1 без пустых границ
  int instruct = tree[tc++];

  element_len = szof(telem);

  if (N == 0)	// обычная переменная int a; или struct point p;
  {
    if (iniproc)
    {
      tocode(STRUCTWITHARR);
      tocode(olddispl);
      tocode(iniprocs[iniproc]);
    }
    if (all)	// int a = или struct{} a =
    {
      if (telem > 0 && modetab[telem] == MSTRUCT)
      {
        Struct_init_gen();
        tocode(COPY0STASS);
        tocode(olddispl);
        tocode(all);	// общее кол-во слов
      }
      else
      {
        Expr_gen(0);
        tocode(telem == LFLOAT ? ASSRV : ASSV);
        tocode(olddispl);
      }
    }
  }
  else	// Обработка массива int a[N1]...[NN] =
  {
    int __abs_N;
    tocode(DEFARR);	// DEFARR N, d, displ, iniproc, usual		N1...NN уже лежат на стеке
    __abs_N = abs(N);
    tocode(all == 0 ? N : __abs_N - 1);
    tocode(element_len);
    tocode(olddispl);
    tocode(iniprocs[iniproc]);
    tocode(usual);
    tocode(all);
    tocode(instruct);

    if (all)	// all == 1, если есть инициализация массива
    {
      Expr_gen(0);
      tocode(ARRINIT);	// ARRINIT N d all displ usual
      tocode(__abs_N);
      tocode(element_len);
      tocode(olddispl);
      tocode(usual);	// == 0 с пустыми границами
              // == 1 без пустых границ и без инициализации
    }
  }
}

void compstmt_gen()
{
  while (tree[tc] != TEnd)
  {
    switch (tree[tc])
    {
      case TDeclarr:
      {
        int i;
        int N;

        tc++;
        N = tree[tc++];

        for (i = 0; i < N; i++)
        {
          Expr_gen(0);
        }
      }
        break;

      case TDeclid:
      {
        tc++;
        Declid_gen();
      }
        break;

      default:
      {
        Stmt_gen();
      }
    }
  }
  tc++;
}

void codegen()
{
  int treesize = tc;
  tc = 0;

  while (tc < treesize)
  {
    switch (tree[tc++])
    {
      case TEnd:
        break;
      case TFuncdef:
      {
        int identref = tree[tc++];
        int _maxdispl = tree[tc++];
        int fn = identab[identref + 3];
        int pred;

        functions[fn] = pc;
        tocode(FUNCBEG);
        tocode(_maxdispl);
        pred = pc++;
        tc++;	// TBegin
        compstmt_gen();
        mem[pred] = pc;
      }
        break;

      case TDeclarr:
      {
        int i;
        int N = tree[tc++];

        for (i = 0; i < N; i++)
        {
          Expr_gen(0);
        }
      }
        break;

      case TDeclid:
        Declid_gen();
        break;

      case NOP:
        break;

      case TStructbeg:
      {
        tocode(B);
        tocode(0);
        iniprocs[tree[tc++]] = pc;
      }
        break;

      case TStructend:
      {
        int numproc = tree[tree[tc++] + 1];
        tocode(STOP);
        mem[iniprocs[numproc] - 1] = pc;
      }
        break;


      default:
      {
        printf("что-то не то\n");
        printf("tc=%i tree[tc-2]=%i tree[tc-1]=%i\n", tc, tree[tc - 2], tree[tc - 1]);
      }
    }
  }

  if (wasmain == 0)
  {
    error(no_main_in_program);
  }

  tocode(CALL1);
  tocode(CALL2);
  tocode(identab[wasmain + 3]);
  tocode(STOP);
}

#endif
