#ifndef H_EXTDECL
#define H_EXTDECL 1

#include "errors.h"
#include "global.h"
#include "scaner.h"

void ext_decl();
int szof(int);

int onlystrings;

void array_init(int);
void block(int);
void expr(int);
void exprassn(int);
void exprassnval();
void exprval();
int gettype();
void struct_init(int);
void unarexpr();


int modeeq(int first_mode, int second_mode)
{
  int n;
  int i;
  int flag = 1;
  int mode;

  if (modetab[first_mode] != modetab[second_mode])
  {
    return 0;
  }

  mode = modetab[first_mode];
  // определяем, сколько полей надо сравнивать для различных типов записей
  if (mode == MSTRUCT || mode == MFUNCTION) 
    n = 2 + modetab[first_mode + 2];
  else 
    n = 1;
  //n = mode == MSTRUCT || mode == MFUNCTION ? 2 + modetab[first_mode + 2] : 1;

  for (i = 1; i <= n && flag; i++)
  {
    flag = modetab[first_mode + i] == modetab[second_mode + i];
  }

  return flag;
}

int check_duplicates()
{
  // проверяет, имеется ли в modetab только что внесенный тип
  // если да, то возвращает ссылку на старую запись, иначе - на новую

  int old = modetab[startmode];

  while (old)
  {
    if (modeeq(startmode + 1, old + 1))
    {
      md = startmode;
      startmode = modetab[startmode];
      return old + 1;
    }
    else
    {
      old = modetab[old];
    }
  }
  return startmode + 1;
}

int newdecl(int _type, int elemtype)
{
  modetab[md] = startmode;
  startmode = md++;
  modetab[md++] = _type;
  modetab[md++] = elemtype; // ссылка на элемент

  return check_duplicates();
}

int evaluate_params(int _num, int formatstr[], int formattypes[], int placeholders[])
{
  int numofparams = 0;
  int i = 0;
  int fsi;

  /*
    for (i=0; i<_num; i++)
    {
      printf("%c %i\n", formatstr[i], formatstr[i]);
    }
  */

  for (i = 0; i < _num; i++)
  {
    if (formatstr[i] == '%')
    {
      if (fsi = formatstr[++i], fsi != '%')
      {
        if (numofparams == MAXPRINTFPARAMS)
        {
          error(too_many_printf_params);
        }

        placeholders[numofparams] = fsi;
      }

      switch (fsi)  // Если добавляется новый спецификатор -
              // - не забыть внести его в switch в bad_printf_placeholder
      {
        case 'i':
          formattypes[numofparams++] = LINT;
          break;
        case 1094:  // 'ц'
          formattypes[numofparams++] = LINT;
          break;

        case 'c':
          formattypes[numofparams++] = LCHAR;
          break;
        case 1083:  // л
          formattypes[numofparams++] = LCHAR;
          break;

        case 'f':
          formattypes[numofparams++] = LFLOAT;
          break;
        case 1074:  // в
          formattypes[numofparams++] = LFLOAT;
          break;

        case 's':
          formattypes[numofparams++] = newdecl(MARRAY, LCHAR);
          break;
        case 1089:  // с
          formattypes[numofparams++] = newdecl(MARRAY, LCHAR);
          break;

        case '%':
          break;

        case 0:
          error(printf_no_format_placeholder);
          break;

        default:
        {
          bad_printf_placeholder = fsi;
          error(printf_unknown_format_placeholder);
        }
          break;
      }
    }
  }

  return numofparams;
}

int szof(int _type)
{
  if (next == LEFTSQBR)
    return 1;
  else if (_type == LFLOAT) 
    return 2;
  else if (_type > 0 && modetab[_type] == MSTRUCT)
    return modetab[_type + 1];
  else
    return 1;
  //return next == LEFTSQBR ? 1 : type == LFLOAT ? 2 : (type > 0 && modetab[type] == MSTRUCT) ? modetab[type + 1] : 1;
}

int is_row_of_char(int t)
{
  return t > 0 && modetab[t] == MARRAY && modetab[t + 1] == LCHAR;
}

int is_function(int t)
{
  return t > 0 && modetab[t] == MFUNCTION;
}

int is_array(int t)
{
  return t > 0 && modetab[t] == MARRAY;
}

int is_pointer(int t)
{
  return t > 0 && modetab[t] == MPOINT;
}

int is_struct(int t)
{
  return t > 0 && modetab[t] == MSTRUCT;
}

int is_float(int t)
{
  return t == LFLOAT || t == LDOUBLE;
}

int is_int(int t)
{
  return t == LINT || t == LLONG || t == LCHAR;
}

void mustbe(int what, int e)
{
  if (scaner() != what)
  {
    error(e);
  }
}

void totree(int opp)
{
  //printf("RuC: Добавлено к дереву %i\n", opp);
  tree[tc++] = opp;
}

void totreef(int opp)
{
  tree[tc++] = opp;

  if (ansttype == LFLOAT &&
    ((opp >= ASS && opp <= DIVASS) || (opp >= ASSAT && opp <= DIVASSAT) || (opp >= EQEQ && opp <= UNMINUS)))
  {
    tree[tc - 1] += 50;
  }
}

int getstatic(int _type)
{
  int olddispl = displ;
  displ += lg * szof(_type); // lg - смещение от l (+1) или от g (-1)

  if (lg > 0)
  {
    if (displ > maxdispl) 
      maxdispl = displ;
    else
      maxdispl = maxdispl;
    // maxdispl = (displ > maxdispl) ? displ : maxdispl;
  }
  else
  {
    maxdisplg = -displ;
  }

  return olddispl;
}

int toidentab(int f, int _type)
{
  // f ==       0, если не ф-ция,
  // f ==       1, если метка,
  // f == funcnum, если описание ф-ции,
  // f ==      -1, если ф-ция-параметр,
  // f >=    1000, если это описание типа,
  // f ==      -2, если #define

  // printf("\n f= %i repr %i rtab[repr] %i rtab[repr+1] %i rtab[repr+2] %i\n", f, repr, reprtab[repr],
  // reprtab[repr+1], reprtab[repr+2]);

  int pred;

  lastid = id;
  if (reprtab[repr + 1] == 0)       // это может быть только MAIN
  {
    if (wasmain)
    {
      error(more_than_1_main);
    }
    wasmain = id;
  }

  pred = identab[id] = reprtab[repr + 1]; // ссылка на описание с таким же представлением в предыдущем блоке
  if (pred)               // pred == 0 только для main, эту ссылку портить нельзя
  {
    reprtab[repr + 1] = id;       // ссылка на текущее описание с этим представлением (это в reprtab)
  }

  if (f != 1 && pred >= curid)      // один и тот же идент м.б. переменной и меткой
  {
    if (func_def == 3 || 
        func_def != 3 && identab[pred + 1] > 0 || 
        func_def != 3 && identab[pred + 1] <= 0 && func_def != 1)
    // if (func_def == 3 ? 1 : identab[pred + 1] > 0 ? 1 : func_def == 1 ? 0 : 1)
    {
      error(repeated_decl); // только определение функции может иметь 2 описания, т.е. иметь предописание
    }
  }

  identab[id + 1] = repr;     // ссылка на представление
  if (f == -2)          // #define
  {
    identab[id + 2] = 1;
    identab[id + 3] = _type;   // это целое число, определенное по #define
  }
  else              // дальше тип или ссылка на modetab (для функций и структур)
  {
    identab[id + 2] = _type;   // тип -1 int, -2 char, -3 float, -4 long, -5 double,
                  // если тип > 0, то это ссылка на modetab

    if (f == 1)
    {
      identab[id + 2] = 0;  // 0, если первым встретился goto, когда встретим метку, поставим 1
      identab[id + 3] = 0;  // при генерации кода когда встретим метку, поставим pc
    }
    else if (f >= 1000)
    {
      identab[id + 3] = f;  // это описание типа, если f > 1000, то f-1000 - это номер иниц проц
    }
    else if (f)
    {
      if (f < 0)
      {
        identab[id + 3] = -(displ++);
        maxdispl = displ;
      }
      else  // identtab[lastid+3] - номер функции, если < 0, то это функция-параметр
      {
        identab[id + 3] = f;
        if (func_def == 2)
        {
          identab[lastid + 1] *= -1;  // это предописание
          predef[++prdf] = repr;
        }
        else
        {
          int i;
          for (i = 0; i <= prdf; i++)
          {
            if (predef[i] == repr)
            {
              predef[i] = 0;
            }
          }
        }
      }
    }
    else
    {
      identab[id + 3] = getstatic(_type);
    }
  }
  id += 4;

  return lastid;
}

void binop(int _sp)
{
  int opp = stackop[_sp];
  int rtype;
  int ltype;

  if (sopnd >= 0) {
    rtype = stackoperands[sopnd];
  }
  else {
    rtype = stack[100+sopnd];
  }
  sopnd--;
  if (sopnd >= 0) {
    ltype = stackoperands[sopnd];
  }
  else {
    ltype = stack[100+sopnd];
  }

  if (is_pointer(ltype) || is_pointer(rtype))
  {
    error(operand_is_pointer);
  }

  if ((opp == LOGOR || opp == LOGAND || opp == LOR || opp == LEXOR || opp == LAND || opp == LSHL || opp == LSHR ||
    opp == LREM) && (is_float(ltype) || is_float(rtype)))
  {
    error(int_op_for_float);
  }

  if (is_int(ltype) && is_float(rtype))
  {
    totree(WIDEN1);
  }

  if (is_int(rtype) && is_float(ltype))
  {
    totree(WIDEN);
  }

  if (is_float(ltype) || is_float(rtype))
  {
    ansttype = LFLOAT;
  }

  if (opp == LOGOR || opp == LOGAND)
  {
    totree(opp);
    tree[stacklog[_sp]] = tc++;
  }
  else
  {
    totreef(opp);
  }

  if (opp >= EQEQ && opp <= LGE)
  {
    ansttype = LINT;
  }

  if (sopnd >= 0) {
    stackoperands[sopnd] = ansttype;
  }
  else {
    stack[100+sopnd] = ansttype;
  }
  // printf("binop sopnd=%i ltype=%i rtype=%i ansttype=%i\n", sopnd, ltype, rtype, ansttype);
  anst = VAL;
}

void toval()  // надо значение положить на стек, например, чтобы передать параметром
{
  if (anst == VAL || anst == NUMBER);
  else if (is_struct(ansttype))
  {
    if (!inass)
    {
      if (anst == IDENT)
      {
        tc -= 2;
        totree(COPY0ST);
        totree(anstdispl);
      }
      else  // тут может быть только ADDR
      {
        totree(COPY1ST);
      }

      totree(modetab[ansttype + 1]);
      anst = VAL;
    }
  }
  else
  {
    if (anst == IDENT)
    {
      if (is_float(ansttype))
        tree[tc - 2] = TIdenttovald;
      else
        tree[tc - 2] = TIdenttoval;
    }

    if (!(is_array(ansttype) || is_pointer(ansttype)))
    {
      if (anst == ADDR)
      {
        if (is_float(ansttype))
          totree(TAddrtovald);
        else
          totree(TAddrtoval);
        //totree(is_float(ansttype) ? TIdenttovald : TIdenttoval);
      }
    }
    anst = VAL;
  }
}

void insertwiden()
{
  tc--;
  totree(WIDEN);
  totree(TExprend);
}

void applid()
{
  lastid = reprtab[repr + 1];
  if (lastid == 1)
  {
    error(ident_is_not_declared);
  }
}

void actstring()
{
  int n = 0;
  int adn;
  totree(TString);
  adn = tc++;
  do
  {
    /*
      if (scaner() == IDENT)
      {
        applid();
        if (identab[lastid+2] == 1)
        {
          cur = NUMBER, ansttype = LINT, num = identab[lastid+3];
        }
      }
    */

    if (scaner() == NUMBER && (ansttype == LINT || ansttype == LCHAR))
    {
      totree(num);
    }
    else
    {
      error(wrong_init_in_actparam);
    }
    ++n;
  } while (scaner() == COMMA);

  tree[adn] = n;
  if (cur != END)
  {
    error(no_comma_or_end);
  }
  ansttype = newdecl(MARRAY, LINT);
  anst = VAL;
}

void mustbestring()
{
  scaner();
  exprassn(1);
  toval();
  sopnd--;

  if (!(ansttype > 0 && modetab[ansttype] == MARRAY && modetab[ansttype + 1] == LCHAR))
  {
    error(not_string_in_stanfunc);
  }
}

void mustbepointstring()
{
  scaner();
  exprassn(1);
  toval();
  sopnd--;

  if (!(ansttype > 0 && modetab[ansttype] == MPOINT && is_array(modetab[ansttype + 1]) &&
    modetab[modetab[ansttype + 1] + 1] == LCHAR))
  {
    error(not_point_string_in_stanfunc);
  }
}

void mustberow()
{
  scaner();
  exprassn(1);
  toval();
  sopnd--;

  if (!(ansttype > 0 && modetab[ansttype] == MARRAY))
  {
    error(not_array_in_stanfunc);
  }
}

void mustbeint()
{
  scaner();
  exprassn(1);
  toval();
  sopnd--;

  if (ansttype != LINT && ansttype != LCHAR)
  {
    error(not_int_in_stanfunc);
  }
}

void mustbeptr()
{
  scaner();
  exprassn(1);
  toval();
  sopnd--;
 
  if (!(ansttype > 0 && modetab[ansttype] == MPOINT))
  {
    error(not_point_string_in_stanfunc);
  }
}

void mustberowofint()
{
  scaner();

  if (cur == BEGIN)
  {
    actstring(), totree(TExprend);
  }
  else
  {
    exprassn(1);
    toval();
    sopnd--;
  }

  if (!(ansttype > 0 && modetab[ansttype] == MARRAY &&
    (modetab[ansttype + 1] == LINT || modetab[ansttype + 1] == LCHAR)))
  {
    error(not_rowofint_in_stanfunc);
  }
}

void primaryexpr()
{
  if (cur == NUMBER)
  {
    if (ansttype == LFLOAT) // ansttype задается прямо в сканере
    {
      totree(TConstd);
      totree(numr.first);
      totree(numr.second);
    }
    else
    {
      totree(TConst);
      totree(num);    // LINT, LCHAR
    }

    ++sopnd;
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
    // printf("number sopnd=%i ansttype=%i\n", sopnd, ansttype);
    anst = NUMBER;
  }
  else if (cur == STRING)
  {
    int i;

    ansttype = newdecl(MARRAY, LCHAR);  // теперь пишем ansttype в анализаторе, а не в сканере
    totree(TString);
    totree(num);

    for (i = 0; i < num; i++)
    {
      totree(lexstr[i]);
    }

    ++sopnd;  // ROWOFCHAR
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
    anst = VAL;
  }
  else if (cur == IDENT)
  {
    applid();
    /*
      if (identab[lastid+2] == 1)     // #define
      {
        totree(TConst);
        totree(num = identab[lastid+3]);
        anst = NUMBER;
        ansttype = LINT;
      }
      else
    */
    {
      totree(TIdent);
      anstdispl = identab[lastid + 3];
      totree(anstdispl);
      ansttype = identab[lastid + 2];
      ++sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
      anst = IDENT;
    }
  }
  else if (cur == LEFTBR)
  {
    if (next == LVOID)
    {
      scaner();
      mustbe(LMULT, no_mult_in_cast);
      unarexpr();

      if (!is_pointer(ansttype))
      {
        error(not_pointer_in_cast);
      }

      mustbe(RIGHTBR, no_rightbr_in_cast);
      toval();
      // totree(CASTC);
      totree(TExprend);
    }
    else
    {
      int oldsp = sp;

      scaner();
      expr(1);
      mustbe(RIGHTBR, wait_rightbr_in_primary);

      while (sp > oldsp)
      {
        binop(--sp);
      }
    }
  }
  else if (cur <= _STANDARD_FUNC_START)    // стандартная функция
  {
    int func = cur;

    if (scaner() != LEFTBR)
    {
      error(no_leftbr_in_stand_func);
    }

    if (func <= _STRCPY && func >= _STRLEN) // функции работы со строками
    {
      if (func >= _STRNCAT)
      {
        mustbepointstring();
      }
      else
      {
        mustbestring();
      }

      if (func != _STRLEN)
      {
        mustbe(COMMA, no_comma_in_act_params_stanfunc);
        mustbestring();

        if (func == _STRNCPY || func == _STRNCAT || func == _STRNCMP)
        {
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbeint();
        }
      }

      if (func < _STRNCAT)
      {
        ansttype = LINT;
        ++sopnd;
        if (sopnd >= 0) {
          stackoperands[sopnd] = ansttype;
        }
        else {
          stack[100+sopnd] = ansttype;
        }
      }
    }
    else if (func >= _ICON && func <= _WIFI_CONNECT)  // функции Фадеева
    {
      notrobot = 0;
      if (func <= _PIXEL && func >= _ICON)
      {
        mustberowofint();
        if (func != _CLEAR)
        {
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
        }

        if (func == _LINE || func == _RECTANGLE || func == _ELLIPS)
        {
          mustbeint();
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbeint();
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbeint();
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbeint();

          if (func != _LINE)
          {
            mustbe(COMMA, no_comma_in_act_params_stanfunc);
            mustbeint();
          }
        }
        else if (func == _ICON || func == _PIXEL)
        {
          mustbeint();
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbeint();

          if (func == _ICON)
          {
            mustbe(COMMA, no_comma_in_act_params_stanfunc);
            mustbeint();
          }
        }
        else if (func == _DRAW_NUMBER || func == _DRAW_STRING)
        {
          mustbeint();
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbeint();
          mustbe(COMMA, no_comma_in_act_params_stanfunc);

          if (func == _DRAW_STRING)
          {
            mustbestring();
          }
          else  // DRAW_NUMBER
          {
            scaner();
            exprassn(1);
            toval();
            sopnd--;
            if (is_int(ansttype))
            {
              totree(WIDEN);
            }
            else if (ansttype != LFLOAT)
            {
              error(not_float_in_stanfunc);
            }
          }
        }
      }
      else if (func == _SETSIGNAL)
      {
        mustbeint();
        mustbe(COMMA, no_comma_in_act_params_stanfunc);
        mustberowofint();
        mustbe(COMMA, no_comma_in_act_params_stanfunc);
        mustberowofint();
      }
      else if (func == _WIFI_CONNECT || func == _BLYNK_AUTORIZATION || func == _BLYNK_NOTIFICATION)
      {
        mustbestring();

        if (func == _WIFI_CONNECT)
        {
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          mustbestring();
        }
      }
      else
      {
        mustbeint();

        if (func != _BLYNK_RECEIVE)
        {
          mustbe(COMMA, no_comma_in_act_params_stanfunc);
          if (func == _BLYNK_TERMINAL)
          {
            mustbestring();
          }
          else if (func == _BLYNK_SEND)
          {
            mustbeint();
          }
          else if (func == _BLYNK_PROPERTY)
          {
            mustbestring();
            mustbe(COMMA, no_comma_in_act_params_stanfunc);
            mustbestring();
          }
          else  // BLYNK_LCD
          {
            mustbeint();
            mustbe(COMMA, no_comma_in_act_params_stanfunc);
            mustbeint();
            mustbe(COMMA, no_comma_in_act_params_stanfunc);
            mustbestring();
          }
        }
        else
        {
          ansttype = LINT;
          ++sopnd;
          if (sopnd >= 0) {
            stackoperands[sopnd] = ansttype;
          }
          else {
            stack[100+sopnd] = ansttype;
          }
        }
      }
    }
    else if (func == _UPB) // UPB
    {
      mustbeint();
      mustbe(COMMA, no_comma_in_act_params_stanfunc);
      mustberow();
      ansttype = LINT;
      ++sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
    }
    else if (func == _FOPEN) // Функции работы с файлами
    {
      mustbestring();
      mustbe(COMMA, no_comma_in_act_params_stanfunc);
      mustbestring();
      ++sopnd;
      if (sopnd >= 0) {
        ansttype = LINT;
        stackoperands[sopnd] = ansttype;
      }
      else {
        ansttype = LCHAR;
        stack[100+sopnd] = ansttype;
      }
      //stackoperands[++sopnd] = ansttype = LINT;
    }
    else if (func == _FCLOSE)
    {
      mustbeint();
    }
    else if (func == _FPUTC)
    {
      mustbeint();
      mustbe(COMMA, no_comma_in_act_params_stanfunc);
      mustbeint();
    }
    else if (func == _FGETC)
    {
      mustbeint();
      ++sopnd;
      if (sopnd >= 0) {
        ansttype = LCHAR;
        stackoperands[sopnd] = ansttype;
      }
      else {
        ansttype = LCHAR;
        stack[100+sopnd] = ansttype;
      }
      //stackoperands[++sopnd] = ansttype = LCHAR;
    }
    else if (func == _PUTC)
    {
      mustbeint();
    }
    else if (func == _GETC) 
    {
      ++sopnd;
      if (sopnd >= 0) {
        ansttype = LCHAR;
        stackoperands[sopnd] = ansttype;
      }
      else {
        ansttype = LCHAR;
        stack[100+sopnd] = ansttype;
      }
      //stackoperands[++sopnd] = ansttype = LCHAR;
    }
    else if (func == _DTONUMR) 
    {
      mustbeptr();
      mustbe(COMMA, no_comma_in_act_params_stanfunc);
      mustbeptr();
    }
    else if (func <= _TMSGSEND && func >= _TGETNUM) // процедуры управления параллельными нитями
    {
      if (func == _TINIT || func == _TDESTROY || func == _TEXIT);  // void()
      else if (func == _TMSGRECEIVE || func == _TGETNUM)      // getnum int()   msgreceive msg_info()
      {
        anst = VAL;
        if (func == _TGETNUM)
        {
          ansttype = LINT;
          ++sopnd;
          if (sopnd >= 0) {
            stackoperands[sopnd] = ansttype;
          }
          else {
            stack[100+sopnd] = ansttype;
          }
        }
        else
        {
          ansttype = 2;
          ++sopnd;
          if (sopnd >= 0) {
            stackoperands[sopnd] = ansttype;
          }
          else {
            stack[100+sopnd] = ansttype;
          }
        }
        //ansttype = stackoperands[++sopnd] = func == _TGETNUM ? LINT : 2;
          // 2 - это ссылка на msg_info
          // не было параметра, выдали 1 результат
      }
      else  // MSGSEND void(msg_info)   CREATE int(void*(*func)(void*))
          // SEMCREATE int(int)   JOIN, SLEEP, SEMWAIT, SEMPOST void(int)
      {
        scaner(); // у этих процедур 1 параметр

        if (func == _TCREATE)
        {
          int dn;

          if (cur != IDENT)
          {
            error(act_param_not_ident);
          }

          applid();

          if (identab[lastid + 2] != 15)  // 15 - это аргумент типа void* (void*)
          {
            error(wrong_arg_in_create);
          }

          ansttype = LINT;
          if (sopnd >= 0) {
            stackoperands[sopnd] = ansttype;
          }
          else {
            stack[100+sopnd] = ansttype;
          }
          dn = identab[lastid + 3];

          if (dn < 0)
          {
            totree(TIdenttoval);
            totree(-dn);
          }
          else
          {
            totree(TConst);
            totree(dn);
          }
          anst = VAL;
        }
        else
        {
          leftansttype = 2;
          exprassn(1);
          toval();

          if (func == _TMSGSEND)
          {
            if (ansttype != 2)  // 2 - это аргумент типа msg_info (struct{int numTh; int data;})
            {
              error(wrong_arg_in_send);
            }
            --sopnd;
          }
          else
          {
            if (!is_int(ansttype))
            {
              error(param_threads_not_int);
            }
            if (func == _TSEMCREATE)
            {
              anst = VAL;
              ansttype = LINT; // съели 1 параметр, выдали int
              if (sopnd >= 0) {
                stackoperands[sopnd] = ansttype;
              }
              else {
                stack[100+sopnd] = ansttype;
              }
            }
            else
            {
              --sopnd;  // съели 1 параметр, не выдали результата
            }
          }
        }
      }
    }
    else if (func == _RAND)
    {
      ansttype = LFLOAT;
      ++sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
    }
    else if (func == _FOPEN) // Функции работы с файлами
    {
      mustbestring();
      mustbe(COMMA, no_comma_in_act_params_stanfunc);
      mustbestring();
      ansttype = LINT;
      ++sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
    }
    else if (func == _FCLOSE)
    {
      mustbeint();
    }
    else if (func == _FPUTC)
    {
      mustbeint();
      mustbe(COMMA, no_comma_in_act_params_stanfunc);
      mustbeint();
    }
    else if (func == _FGETC)
    {
      mustbeint();
      ansttype = LCHAR;
      ++sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
    }
    else if (func == _PUTC)
    {
      mustbeint();
    }
    else if (func == _GETC) 
    {
      ansttype = LCHAR;
      ++sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
    }
    else
    {
      scaner();
      exprassn(1);
      toval();

      // GETDIGSENSOR int(int port, int pins[]), GETANSENSOR int (int port, int pin)
      // SETMOTOR и VOLTAGE void (int port, int volt)
      if (func == _GETDIGSENSOR || func == _GETANSENSOR || func == _SETMOTOR || func == _VOLTAGE)
      {
        notrobot = 0;
        if (!is_int(ansttype))
        {
          error(param_setmotor_not_int);
        }

        mustbe(COMMA, no_comma_in_setmotor);
        scaner();

        if (func == _GETDIGSENSOR)
        {
          if (cur == BEGIN)
          {
            sopnd--, actstring();
          }
          else
          {
            error(getdigsensorerr);
          }
        }
        else
        {
          exprassn(1);
          toval();

          if (!is_int(ansttype))
          {
            error(param_setmotor_not_int);
          }
          if (func == _SETMOTOR || func == _VOLTAGE)
          {
            sopnd -= 2;
          }
          else
          {
            --sopnd, anst = VAL;
          }
        }

        if (func == _SETMOTOR || func == _VOLTAGE)
        {
          sopnd -= 2;
        }
        else
        {
          anst = VAL, --sopnd;
        }
      }
      else if (func == _ABS && is_int(ansttype))
      {
        func = _ABSI;
      }
      else
      {
        if (is_int(ansttype))
        {
          totree(WIDEN);
          ansttype = LFLOAT;
          if (sopnd >= 0) {
            stackoperands[sopnd] = ansttype;
          }
          else {
            stack[100+sopnd] = ansttype;
          }
        }

        if (!is_float(ansttype))
        {
          error(bad_param_in_stand_func);
        }

        if (func == _ROUND)
        {
          ansttype = LINT;
          if (sopnd >= 0) {
            stackoperands[sopnd] = ansttype;
          }
          else {
            stack[100+sopnd] = ansttype;
          }
        }
      }
    }
    totree(9500 - func);
    mustbe(RIGHTBR, no_rightbr_in_stand_func);
  }
  else
  {
    error(not_primary);
  }
}

void index_check()
{
  if (!is_int(ansttype))
  {
    error(index_must_be_int);
  }
}

int find_field(int stype) // выдает смещение до найденного поля или ошибку
{
  int i;
  int flag = 1;
  int select_displ = 0;

  scaner();
  mustbe(IDENT, after_dot_must_be_ident);

  for (i = 0; i < modetab[stype + 2]; i += 2) // тут хранится удвоенное n
  {
    int field_type = modetab[stype + 3 + i];

    if (modetab[stype + 4 + i] == repr)
    {
      ansttype = field_type;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
      //stackoperands[sopnd] = ansttype = field_type;
      flag = 0;
      break;
    }
    else
    {
      select_displ += szof(field_type);
    }
    // прибавляем к суммарному смещению длину поля
  }

  if (flag)
  {
    error(no_field);
  }

  return select_displ;
}

void selectend()
{
  while (next == DOT)
  {
    anstdispl += find_field(ansttype);
  }

  totree(anstdispl);
  if (is_array(ansttype) || is_pointer(ansttype))
  {
    totree(TAddrtoval);
  }
}

int Norder(int t) // вычислить размерность массива
{
  int n = 1;

  while ((t = modetab[t + 1]) > 0)
  {
    n++;
  }

  return n;
}

void postexpr()
{
  int lid;
  int leftansttyp;
  int was_func = 0;

  lid = lastid;
  leftansttyp = ansttype;

  if (next == LEFTBR) // вызов функции
  {
    int i;
    int j;
    int n;
    int dn;
    int oldinass = inass;

    was_func = 1;
    scaner();

    if (!is_function(leftansttyp))
    {
      error(call_not_from_function);
    }

    n = modetab[leftansttyp + 2]; // берем количество аргументов функции

    totree(TCall1);
    totree(n);
    j = leftansttyp + 3;

    for (i = 0; i < n; i++)     // фактические параметры
    {
      int mdj = leftansttype = modetab[j];
        // это вид формального параметра, в ansttype будет вид фактического параметра
      scaner();

      if (is_function(mdj))   // фактическим параметром должна быть функция, в С - это только идентификатор
      {
        if (cur != IDENT)
        {
          error(act_param_not_ident);
        }
        applid();

        if (identab[lastid + 2] != mdj)
        {
          error(diff_formal_param_type_and_actual);
        }
        dn = identab[lastid + 3];

        if (dn < 0)
        {
          totree(TIdenttoval);
          totree(-dn);
        }
        else
        {
          totree(TConst);
          totree(dn);
        }
        totree(TExprend);
      }
      else
      {
        if (cur == BEGIN && is_array(mdj))
        {
          actstring(), totree(TExprend);
        }
        else
        {
          inass = 0;
          exprassn(1);
          toval();
          totree(TExprend);

          if (mdj > 0 && mdj != ansttype)
          {
            error(diff_formal_param_type_and_actual);
          }

          if (is_int(mdj) && is_float(ansttype))
          {
            error(float_instead_int);
          }

          if (is_float(mdj) && is_int(ansttype))
          {
            insertwiden();
          }
          --sopnd;
        }
      }
      if (i < n - 1 && scaner() != COMMA)
      {
        error(no_comma_in_act_params);
      }
      j++;
    }

    inass = oldinass;
    mustbe(RIGHTBR, wrong_number_of_params);
    totree(TCall2);
    totree(lid);
    ansttype = modetab[leftansttyp + 1];
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
    anst = VAL;

    if (is_struct(ansttype))
    {
      x -= modetab[ansttype + 1] - 1;
    }
  }

  while (next == LEFTSQBR || next == ARROW || next == DOT)
  {
    while (next == LEFTSQBR)  // вырезка из массива (возможно, многомерного)
    {
      int elem_type;

      if (was_func)
      {
        error(slice_from_func);
      }

      if (modetab[ansttype] != MARRAY)    // вырезка не из массива
      {
        error(slice_not_from_array);
      }

      elem_type = modetab[ansttype + 1];

      scaner();
      scaner();

      if (anst == IDENT)            // a[i]
      {
        tree[tc - 2] = TSliceident;
        tree[tc - 1] = anstdispl;
      }
      else                  // a[i][j]
      {
        totree(TSlice);
      }

      totree(elem_type);
      exprval();
      index_check();              // проверка, что индекс int или char
      mustbe(RIGHTSQBR, no_rightsqbr_in_slice);

      ansttype = elem_type;
      --sopnd;
      if (sopnd >= 0) {
        stackoperands[sopnd] = ansttype;
      }
      else {
        stack[100+sopnd] = ansttype;
      }
      anst = ADDR;
    }

    while (next == ARROW)
      // это выборка поля из указателя на структуру, если после ->
      // больше одной точки подряд, схлопываем в 1 select
      // перед выборкой мог быть вызов функции или вырезка элемента массива
    {
      if (modetab[ansttype] != MPOINT || modetab[modetab[ansttype + 1]] != MSTRUCT)
      {
        error(get_field_not_from_struct_pointer);
      }

      if (anst == IDENT)
      {
        tree[tc - 2] = TIdenttoval;
      }
      anst = ADDR;    // pointer мог быть значением функции (VAL) или, может быть,
      totree(TSelect);  // anst уже был ADDR, т.е. адрес теперь уже всегда на верхушке стека

      anstdispl = find_field(ansttype = modetab[ansttype + 1]);
      selectend();
    }

    if (next == DOT)
    {
      /*
        int i;
        for (i = 3800; i < 3850; ++i)
        {
          printf("%i) reprtab[i]= %i %c\n", i, reprtab[i], reprtab[i]);
        }
      */

      if (ansttype < 0 || modetab[ansttype] != MSTRUCT)
      {
        error(select_not_from_struct);
      }

      if (anst == VAL)  // структура - значение функции
      {
        int len1 = szof(ansttype);
        int sz = 0;

        anstdispl = 0;
        while (next == DOT)
        {
          anstdispl += find_field(ansttype);
        }

        totree(COPYST);
        totree(anstdispl);
        //sz = szof(ansttype);
        totree(len1);
        //totree(szof(ansttype));
        // PROBABLY AN ERROR - NOT YET SURE.
        // totree(len1);
      }
      else if (anst == IDENT)
      {
        int globid;
        if (anstdispl < 0)
          globid = -1;
        else
          globid = 1;
        
        //int globid = anstdispl < 0 ? -1 : 1;

        while (next == DOT)
        {
          anstdispl += globid * find_field(ansttype);
        }

        tree[tc - 1] = anstdispl;
      }
      else  // ADDR
      {
        totree(TSelect);
        anstdispl = 0;
        selectend();
      }
    }
  }

  if (next == INC || next == DEC) // a++, a--
  {
    int opp;

    if (!is_int(ansttype) && !is_float(ansttype))
    {
      error(wrong_operand);
    }

    if (anst != IDENT && anst != ADDR)
    {
      error(unassignable_inc);
    }

    if (next == INC) 
      opp = POSTINC;
    else
      opp = POSTDEC;
    //opp = (next == INC) ? POSTINC : POSTDEC;
    if (anst == ADDR)
    {
      opp += 4;
    }
    scaner();
    totreef(opp);

    if (anst == IDENT)
    {
      totree(identab[lid + 3]);
    }
    anst = VAL;
  }
}

void unarexpr()
{
  int opp = cur;

  if (cur == LNOT || cur == LOGNOT || cur == LPLUS || cur == LMINUS || cur == LAND || cur == LMULT || cur == INC ||
    cur == DEC)
  {
    if (cur == INC || cur == DEC)
    {
      scaner();
      unarexpr();

      if (anst != IDENT && anst != ADDR)
      {
        error(unassignable_inc);
      }

      if (anst == ADDR)
      {
        opp += 4;
      }
      totreef(opp);

      if (anst == IDENT)
      {
        totree(identab[lastid + 3]);
      }
      anst = VAL;
    }
    else
    {
      scaner();
      unarexpr();

      if (opp == LAND)
      {
        if (anst == VAL)
        {
          error(wrong_addr);
        }

        if (anst == IDENT)
        {
          tree[tc - 2] = TIdenttoaddr;  // &a
        }

        ansttype = newdecl(MPOINT, ansttype);
        if (sopnd >= 0) {
          stackoperands[sopnd] = ansttype;
        }
        else {
          stack[100+sopnd] = ansttype;
        }
        anst = VAL;
      }
      else if (opp == LMULT)
      {
        if (!is_pointer(ansttype))
        {
          error(aster_not_for_pointer);
        }

        if (anst == IDENT)
        {
          tree[tc - 2] = TIdenttoval;   // *p
        }

        ansttype = modetab[ansttype + 1];
        if (sopnd >= 0) {
          stackoperands[sopnd] = ansttype;
        }
        else {
          stack[100+sopnd] = ansttype;
        }

        //stackoperands[sopnd] = ansttype = modetab[ansttype + 1];
        anst = ADDR;
      }
      else
      {
        toval();

        if ((opp == LNOT || opp == LOGNOT) && ansttype == LFLOAT)
        {
          error(int_op_for_float);
        }
        else if (opp == LMINUS)
        {
          totreef(UNMINUS);
        }
        else if (opp == LPLUS);
        else
        {
          totree(opp);
        }
        anst = VAL;
      }
    }
  }
  else
  {
    primaryexpr();
  }
  postexpr();
  if (sopnd >= 0) {
    stackoperands[sopnd] = ansttype;
  }
  else {
    stack[100+sopnd] = ansttype;
  }
}

void exprinbrkts(int er)
{
  mustbe(LEFTBR, er);
  scaner();
  exprval();
  mustbe(RIGHTBR, er);
}

void exprassninbrkts(int er)
{
  mustbe(LEFTBR, er);
  scaner();
  exprassnval();
  mustbe(RIGHTBR, er);
}

int prio(int opp)  // возвращает 0, если не операция
{
  if (opp == LOGOR)
    return 1;
  else if (opp == LOGAND)
    return 2;
  else if (opp == LOR)
    return 3;
  else if (opp == LEXOR)
    return 4;
  else if (opp == LAND)
    return 5;
  else if (opp == EQEQ)
    return 6;
  else if (opp == NOTEQ)
    return 6;
  else if (opp == LLT)
    return 7;
  else if (opp == LGT)
    return 7;
  else if (opp == LLE)
    return 7;
  else if (opp == LGE)
    return 7;
  else if (opp == LSHL)
    return 8;
  else if (opp == LSHR)
    return 8;
  else if (opp == LPLUS)
    return 9;
  else if (opp == LMINUS)
    return 9;
  else if (opp == LMULT)
    return 10;
  else if (opp == LDIV)
    return 10;
  else if (opp == LREM)
    return 10;
  else 
    return 0;
  // return opp == LOGOR
  //       ? 1
  //       : opp == LOGAND
  //         ? 2
  //         : opp == LOR
  //           ? 3
  //           : opp == LEXOR
  //             ? 4
  //             : opp == LAND
  //               ? 5
  //               : opp == EQEQ
  //                 ? 6
  //                 : opp == NOTEQ
  //                   ? 6
  //                   : opp == LLT
  //                     ? 7
  //                     : opp == LGT
  //                       ? 7
  //                       : opp == LLE
  //                         ? 7
  //                         : opp == LGE
  //                           ? 7
  //                           : opp == LSHL
  //                             ? 8
  //                             : opp == LSHR
  //                               ? 8
  //                               : opp == LPLUS
  //                                 ? 9
  //                                 : opp == LMINUS
  //                                   ? 9
  //                                   : opp == LMULT
  //                                     ? 10
  //                                     : opp == LDIV
  //                                       ? 10
  //                                       : opp == LREM
  //                                         ? 10
  //                                         : 0;
}

void subexpr()
{
  int p;
  int oldsp = sp;
  int wasop = 0;
  int ad = 0;

  while ((p = prio(next)))
  {
    wasop = 1;
    toval();

    while (sp > oldsp && stack[sp - 1] >= p)
    {
      binop(--sp);
    }

    if (p <= 2)
    {
      if (p == 1)
        totree(ADLOGOR);
      else
        totree(ADLOGAND);
      //totree(p == 1 ? ADLOGOR : ADLOGAND);
      ad = tc++;
    }

    stack[sp] = p;
    stacklog[sp] = ad;
    stackop[sp++] = next;
    scaner();
    scaner();
    unarexpr();
  }

  if (wasop)
  {
    toval();
  }

  while (sp > oldsp)
  {
    //sp = sp - 1;
    binop(--sp);
  }
}

int intopassn(int next2)
{
  return next2 == REMASS || next2 == SHLASS || next2 == SHRASS || next2 == ANDASS || next2 == EXORASS || next2 == ORASS;
}

int opassn()
{
  if (next == ASS || next == MULTASS || next == DIVASS || next == PLUSASS || next == MINUSASS || intopassn(next)) {
    op = next;
    //printf("Debgu: i %i\n", op);
    //printid(op);
    return op;
  } else
    return 0;
  // return (next == ASS || next == MULTASS || next == DIVASS || next == PLUSASS || next == MINUSASS || intopassn(next))
  //       ? op = next
  //       : 0;
}

void condexpr()
{
  int globtype = 0;
  int adif = 0;
  int r;

  subexpr();  // logORexpr();
  if (next == QUEST)
  {
    while (next == QUEST)
    {
      toval();

      if (!is_int(ansttype))
      {
        error(float_in_condition);
      }

      totree(TCondexpr);
      scaner();
      scaner();
      sopnd--;
      exprval();  // then

      if (!globtype)
      {
        globtype = ansttype;
      }
      sopnd--;

      if (is_float(ansttype))
      {
        globtype = LFLOAT;
      }
      else
      {
        tree[tc] = adif;
        adif = tc++;
      }

      mustbe(COLON, no_colon_in_cond_expr);
      scaner();
      unarexpr();
      subexpr();  // logORexpr();   else or elif
    }

    toval();
    totree(TExprend);

    if (is_float(ansttype))
    {
      globtype = LFLOAT;
    }
    else
    {
      tree[tc] = adif;
      adif = tc++;
    }

    while (adif != 0)
    {
      r = tree[adif];
      tree[adif] = TExprend;
      if (is_float(globtype))
        tree[adif - 1] = WIDEN;
      else
        tree[adif - 1] = NOP;

      //tree[adif - 1] = is_float(globtype) ? WIDEN : NOP;
      adif = r;
    }
    ansttype = globtype;
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
  }
  else
  {
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
  }
}

void inition(int decl_type)
{
  if (decl_type < 0 || is_pointer(decl_type) ||         // Обработка для базовых типов, указателей
    (is_array(decl_type) && modetab[decl_type + 1] == LCHAR)) // или строк
  {
    exprassn(1);
    toval();
    totree(TExprend);
    // съедаем выражение, его значение будет на стеке
    sopnd--;

    if (is_int(decl_type) && is_float(ansttype))
    {
      error(init_int_by_float);
    }

    if (is_float(decl_type) && is_int(ansttype))
    {
      insertwiden();
    }
    else if (decl_type != ansttype)
    {
      error(error_in_initialization);
    }
  }
  else if (cur == BEGIN)
  {
    struct_init(decl_type);
  }
  else
  {
    error(wrong_init);
  }
}

void struct_init(int decl_type) // сейчас modetab[decl_type] равен MSTRUCT
{
  int next_field = decl_type + 3;
  int i;
  int nf = modetab[decl_type + 2] / 2;

  if (cur != BEGIN)
  {
    error(struct_init_must_start_from_BEGIN);
  }
  totree(TStructinit);
  totree(nf);

  for (i = 0; i < nf; i++)
  {
    scaner();
    inition(modetab[next_field]);
    next_field += 2;

    if (i != nf - 1)
    {
      if (next == COMMA)  // поля инициализации идут через запятую, заканчиваются }
      {
        scaner();
      }
      else
      {
        error(no_comma_in_init_list);
      }
    }
  }

  if (next == END)
  {
    totree(TExprend);
  }
  else
  {
    error(wait_end);
  }

  scaner();
  leftansttype = decl_type;
}


void exprassnvoid()
{
  int t;
  int tt;

  if (tree[tc - 2] < 9000)
    t = tc - 3; 
  else
    t = tc - 2;
  //int t = tree[tc - 2] < 9000 ? tc - 3 : tc - 2;
  tt = tree[t];

  if ((tt >= ASS && tt <= DIVASSAT) || (tt >= POSTINC && tt <= DECAT) || (tt >= ASSR && tt <= DIVASSATR) ||
    (tt >= POSTINCR && tt <= DECATR))
  {
    tree[t] += 200;
  }
  --sopnd;
}

void exprassn(int level)
{
  int leftanst;
  int leftanstdispl;
  int ltype;
  int rtype;
  int lnext;

  if (cur == BEGIN)
  {
    if (is_struct(leftansttype))
    {
      struct_init(leftansttype);
    }
    /*
      else if (is_array(leftansttype))  //пока в RuC присваивать массивы нельзя
      {
        array_init(leftansttype);
      }
    */
    else
    {
      error(init_not_struct);
    }
    
    ansttype = leftansttype;
    ++sopnd;
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
    anst = VAL;
  }
  else
  {
    unarexpr();
  }

  leftanst = anst;
  leftanstdispl = anstdispl;
  leftansttype = ansttype;
  if (opassn())
  {
    int opp = op;

    lnext = next;
    inass = 1;
    scaner();
    scaner();
    exprassn(level + 1);
    inass = 0;

    if (leftanst == VAL)
    {
      error(unassignable);
    }
    if (sopnd >= 0) {
      rtype = stackoperands[sopnd];
    }
    else {
      rtype = stack[100 + sopnd];
    } // снимаем типы операндов со стека
    sopnd--;
    if (sopnd >= 0) {
      ltype = stackoperands[sopnd];
    }
    else {
      ltype = stack[100 + sopnd];
    } 

    if (intopassn(lnext) && (is_float(ltype) || is_float(rtype)))
    {
      error(int_op_for_float);
    }

    if (is_array(ltype))      // присваивать массив в массив в си нельзя
    {
      error(array_assigment);
    }

    if (is_struct(ltype))     // присваивание в структуру
    {
      if (ltype != rtype)     // типы должны быть равны
      {
        error(type_missmatch);
      }

      if (opp != ASS)       // в структуру можно присваивать только с помощью =
      {
        error(wrong_struct_ass);
      }

      if (anst == VAL)
      {
        if (leftanst == IDENT)
          opp = COPY0STASS;
        else 
          opp = COPY1STASS;
        //opp = leftanst == IDENT ? COPY0STASS : COPY1STASS;
      }
      else
      {
        if (leftanst == IDENT)
        {
          if (anst == IDENT)
            opp = COPY00;
          else
            opp = COPY01;
        }
        else {
          if (anst == IDENT)
            opp = COPY10;
          else
            opp = COPY11;
        }
        //opp = leftanst == IDENT ? anst == IDENT ? COPY00 : COPY01 : anst == IDENT ? COPY10 : COPY11;
      }
      totree(opp);

      if (leftanst == IDENT)
      {
        totree(leftanstdispl);  // displleft
      }

      if (anst == IDENT)
      {
        totree(anstdispl);    // displright
      }
      totree(modetab[ltype + 1]); // длина
      anst = leftanst;
      anstdispl = leftanstdispl;
    }
    else  // оба операнда базового типа или указатели
    {
      if (is_pointer(ltype) && opp != ASS)  // в указатель можно присваивать только с помощью =
      {
        error(wrong_struct_ass);
      }

      if (is_int(ltype) && is_float(rtype))
      {
        error(assmnt_float_to_int);
      }

      toval();
      if (is_int(rtype) && is_float(ltype))
      {
        totree(WIDEN);
        ansttype = LFLOAT;
      }

      if (is_pointer(ltype) && is_pointer(rtype) && ltype != rtype) // проверка нужна только для указателей
      {
        error(type_missmatch);
      }

      if (leftanst == ADDR)
      {
        opp += 11;
      }
      totreef(opp);

      if (leftanst == IDENT)
      {
        totree(anstdispl = leftanstdispl);
      }
      anst = VAL;
    }
    ansttype = ltype; // тип результата - на стек
    if (sopnd >= 0) {
      stackoperands[sopnd] = ansttype;
    }
    else {
      stack[100+sopnd] = ansttype;
    }
  }
  else
  {
    condexpr(); // condexpr учитывает тот факт, что начало выражения в виде unarexpr уже выкушано
  }
}

void expr(int level)
{
  exprassn(level);

  while (next == COMMA)
  {
    exprassnvoid();
    sopnd--;
    scaner();
    scaner();
    exprassn(level);
  }

  if (level == 0)
  {
    totree(TExprend);
  }
}

void exprval()
{
  expr(1);
  toval();
  totree(TExprend);
}

void exprassnval()
{
  exprassn(1);
  toval();
  totree(TExprend);
}

void array_init(int decl_type)  // сейчас modetab[decl_type] равен MARRAY
{
  int ad;
  int all = 0;

  if (is_array(decl_type))
  {
    if (cur == STRING)
    {
      if (onlystrings == 0)
      {
        error(string_and_notstring);
      }

      if (onlystrings == 2)
      {
        onlystrings = 1;
      }

      primaryexpr();
      totree(TExprend);
    }
    else if (cur == BEGIN)
    {
      totree(TBeginit);
      ad = tc++;

      do
      {
        scaner();
        all++;
        array_init(modetab[decl_type + 1]);
      } while (scaner() == COMMA);

      if (cur == END)
      {
        tree[ad] = all;
        totree(TExprend);
      }
      else
      {
        error(wait_end);
      }
    }
    else
    {
      error(arr_init_must_start_from_BEGIN);
    }
  }
  else if (cur == BEGIN)
  {
    if (is_struct(decl_type))
    {
      struct_init(decl_type);
    }
    else
    {
      error(begin_with_notarray);
    }
  }
  else if (onlystrings == 1)
  {
    error(string_and_notstring);
  }
  else
  {
    inition(decl_type);
    onlystrings = 0;
  }
}

int arrdef(int t) // вызывается при описании массивов и структур из массивов сразу после idorpnt
{
  arrdim = 0;
  usual = 1;    // описание массива без пустых границ
  if (is_pointer(t))
  {
    error(pnt_before_array);
  }

  while (next == LEFTSQBR)  // это определение массива (может быть многомерным)
  {
    arrdim++;
    scaner();

    if (next == RIGHTSQBR)
    {
      scaner();
      if (next == LEFTSQBR) // int a[][]={{1,2,3}, {4,5,6}} - нельзя;
      {
        error(empty_init);  // границы определять по инициализации можно только по последнему изм.
      }
      usual = 0;
    }
    else
    {
      scaner();
      unarexpr();
      condexpr();
      toval();

      if (!is_int(ansttype))
      {
        error(array_size_must_be_int);
      }

      totree(TExprend);
      sopnd--;
      mustbe(RIGHTSQBR, wait_right_sq_br);
    }
    t = newdecl(MARRAY, t);   // Меняем тип в identtab (увеличиваем размерность массива)
  }
  return t;
}


void decl_id(int decl_type)
{
  // вызывается из block и extdecl, только эта процедура реально отводит память
  // если встретятся массивы (прямо или в структурах), их размеры уже будут в стеке

  int oldid = toidentab(0, decl_type);
  int elem_len;
  int elem_type;
  int all;  // all - место в дереве, где будет общее количество выражений в инициализации, для массивов - только
        // признак (1) наличия инициализации
  int adN;

  usual = 1;
  arrdim = 0; // arrdim - размерность (0-скаляр), д.б. столько выражений-границ
  elem_type = decl_type;

  if (next == LEFTSQBR) // это определение массива (может быть многомерным)
  {
    totree(TDeclarr);
    adN = tc++;
    elem_len = szof(decl_type);
    decl_type = identab[oldid + 2] = arrdef(decl_type); // Меняем тип (увеличиваем размерность массива)
    tree[adN] = arrdim;

    if (!usual && next != ASS)
    {
      error(empty_bound_without_init);
    }
  }

  totree(TDeclid);
  totree(identab[oldid + 3]);                   // displ
  totree(elem_type);                        // elem_type
  totree(arrdim);                         // N
  tree[all = tc++] = 0;                     // all
  if (is_pointer(decl_type))
    tree[tc++] = 0; // proc
  else
    tree[tc++] = was_struct_with_arr; // proc
  // tree[tc++] = is_pointer(decl_type) ? 0 : was_struct_with_arr; // proc
  totree(usual);                          // usual
  totree(0);                            // массив не в структуре

  if (next == ASS)
  {
    scaner();
    scaner();
    tree[all] = szof(decl_type);

    if (is_array(decl_type))      // инициализация массива
    {
      onlystrings = 2;

      if (!usual)
      {
        tree[adN]--;        // это уменьшение N в Declarr
      }

      array_init(decl_type);

      if (onlystrings == 1)
      {
        tree[all + 2] = usual + 2;  // только из строк 2 - без границ, 3 - с границами
      }
    }
    else
    {
      inition(decl_type);
    }
  }
}

void statement()
{
  int flagsemicol = 1;
  int oldwasdefault = wasdefault;
  int oldinswitch = inswitch;
  int oldinloop = inloop;

  wasdefault = 0;
  scaner();

  if ((is_int(cur) || is_float(cur) || cur == LVOID || cur == LSTRUCT) && blockflag)
  {
    error(decl_after_strmt);
  }

  if (cur == BEGIN)
  {
    flagsemicol = 0;
    block(1);
  }
  else if (cur == _TCREATEDIRECT)
  {
    totree(CREATEDIRECTC);
    flagsemicol = 0;
    block(2);
    totree(EXITC);
  }
  else if (cur == SEMICOLON)
  {
    totree(NOP);
    flagsemicol = 0;
  }
  else if (cur == IDENT && next == COLON)
  {
    int _id;
    int i;
    int flag = 1;

    flagsemicol = 0;
    totree(TLabel);

    for (i = 0; flag && i < pgotost - 1; i += 2)
    {
      flag = identab[gotost[i] + 1] != repr;
    }

    if (flag)
    {
      _id = toidentab(1, 0);
      totree(_id);
      gotost[pgotost++] = _id;     // это определение метки, если она встретилась до переходов на нее
      gotost[pgotost++] = -line;
    }
    else
    {
      _id = gotost[i - 2];
      repr = identab[id + 1];

      if (gotost[i - 1] < 0)
      {
        error(repeated_label);
      }
      totree(_id);
    }
    identab[_id + 2] = 1;

    scaner();
    statement();
  }
  else
  {
    blockflag = 1;

    switch (cur)
    {
      case _PRINT:
      {
        exprassninbrkts(print_without_br);
        tc--;
        totree(TPrint);
        totree(ansttype);
        totree(TExprend);

        if (is_pointer(ansttype))
        {
          error(pointer_in_print);
        }
        sopnd--;
      }
        break;
      case _PRINTID:
      {
        int _foo1 = 1;
        mustbe(LEFTBR, no_leftbr_in_printid);
        do
        {       
          mustbe(IDENT, no_ident_in_printid);
          lastid = reprtab[repr + 1];

          if (lastid == 1)
          {
            error(ident_is_not_declared);
          }

          totree(TPrintid);
          totree(lastid);

          if (next == COMMA)
            scaner();
          else
            _foo1 = 0;
        } while (_foo1);
        //} while (next == COMMA ? scaner(), 1 : 0);
        mustbe(RIGHTBR, no_rightbr_in_printid);
      }
        break;

      case _PRINTF:
      {
        int formatstr[MAXSTRINGL];
        int formattypes[MAXPRINTFPARAMS];
        int placeholders[MAXPRINTFPARAMS];
        int paramnum = 0;
        int sumsize = 0;
        int i = 0;
        int fnum;

        mustbe(LEFTBR, no_leftbr_in_printf);
        if (scaner() != STRING) //выкушиваем форматную строку
        {
          error(wrong_first_printf_param);
        }

        for (i = 0; i < num; i++)
        {
          formatstr[i] = lexstr[i];
        }
        formatstr[num] = 0;

        paramnum = evaluate_params(fnum = num, formatstr, formattypes, placeholders);

        for (i = 0; scaner() == COMMA; i++)
        {
          if (i >= paramnum)
          {
            error(wrong_printf_param_number);
          }

          scaner();

          exprassn(1);
          toval();
          totree(TExprend);

          if (formattypes[i] == LFLOAT && ansttype == LINT)
          {
            insertwiden();
          }
          else if (formattypes[i] != ansttype)
          {
            bad_printf_placeholder = placeholders[i];
            error(wrong_printf_param_type);
          }

          sumsize += szof(formattypes[i]);
          --sopnd;
        }

        if (cur != RIGHTBR)
        {
          error(no_rightbr_in_printf);
        }

        if (i != paramnum)
        {
          error(wrong_printf_param_number);
        }

        totree(TString);
        totree(fnum);

        for (i = 0; i < fnum; i++)
        {
          totree(formatstr[i]);
        }
        totree(TExprend);

        totree(TPrintf);
        totree(sumsize);
      }
        break;

      case _GETID:
      {
        int _foo2 = 1;
        mustbe(LEFTBR, no_leftbr_in_printid);
        do
        {
          mustbe(IDENT, no_ident_in_printid);
          lastid = reprtab[repr + 1];

          if (lastid == 1)
          {
            error(ident_is_not_declared);
          }

          totree(TGetid);
          totree(lastid);

          if (next == COMMA)
            scaner();
          else
            _foo2 = 0;

        } while (_foo2);
        //} while (next == COMMA ? scaner(), 1 : 0);
        mustbe(RIGHTBR, no_rightbr_in_printid);
      }
        break;
      case _FPRINTF:
      {
        int formatstr[MAXSTRINGL];
        int formattypes[MAXPRINTFPARAMS];
        int placeholders[MAXPRINTFPARAMS];
        int paramnum = 0;
        int sumsize = 0;
        int i = 0;
        int fnum;

        mustbe(LEFTBR, no_leftbr_in_printf);
        mustbeint();
        mustbe(COMMA, no_comma_in_act_params_stanfunc);

        if (scaner() != STRING) //выкушиваем форматную строку
        {
          error(wrong_first_printf_param);
        }

        for (i = 0; i < num; i++)
        {
          formatstr[i] = lexstr[i];
        }
        formatstr[num] = 0;

        paramnum = evaluate_params(fnum = num, formatstr, formattypes, placeholders);

        for (i = 0; scaner() == COMMA; i++)
        {
          if (i >= paramnum)
          {
            error(wrong_printf_param_number);
          }

          scaner();

          exprassn(1);
          toval();
          totree(TExprend);

          if (formattypes[i] == LFLOAT && ansttype == LINT)
          {
            insertwiden();
          }
          else if (formattypes[i] != ansttype)
          {
            bad_printf_placeholder = placeholders[i];
            error(wrong_printf_param_type);
          }

          sumsize += szof(formattypes[i]);
          --sopnd;
        }

        if (cur != RIGHTBR)
        {
          error(no_rightbr_in_printf);
        }
 
        if (i != paramnum)
        {
          error(wrong_printf_param_number);
        }

        totree(TString);
        totree(fnum);

        for (i = 0; i < fnum; i++)
        {
          totree(formatstr[i]);
        }
        totree(TExprend);

        totree(TFprintf);
        totree(sumsize);
      }
        break;
      case LBREAK:
      {
        if (!(inloop || inswitch))
        {
          error(break_not_in_loop_or_switch);
        }
        totree(TBreak);
      }
        break;
      case LCASE:
      {
        if (!inswitch)
        {
          error(case_or_default_not_in_switch);
        }

        if (wasdefault)
        {
          error(case_after_default);
        }

        totree(TCase);
        scaner();
        unarexpr();
        condexpr();
        toval();
        totree(TExprend);

        if (ansttype == LFLOAT)
        {
          error(float_in_switch);
        }

        sopnd--;
        mustbe(COLON, no_colon_in_case);
        flagsemicol = 0;
        statement();
      }
        break;
      case LCONTINUE:
      {
        if (!inloop)
        {
          error(continue_not_in_loop);
        }
        totree(TContinue);
      }
        break;
      case LDEFAULT:
      {
        if (!inswitch)
        {
          error(case_or_default_not_in_switch);
        }

        mustbe(COLON, no_colon_in_case);
        wasdefault = 1;
        flagsemicol = 0;
        totree(TDefault);
        statement();
      }
        break;
      case LDO:
      {
        inloop = 1;
        totree(TDo);
        statement();

        if (next == LWHILE)
        {
          scaner();
          exprinbrkts(cond_must_be_in_brkts);
          sopnd--;
        }
        else
        {
          error(wait_while_in_do_stmt);
        }
      }
        break;
      case LFOR:
      {
        int fromref;
        int condref;
        int incrref;
        int stmtref;

        mustbe(LEFTBR, no_leftbr_in_for);
        totree(TFor);
        fromref = tc++;
        condref = tc++;
        incrref = tc++;
        stmtref = tc++;

        if (scaner() == SEMICOLON)  // init
        {
          tree[fromref] = 0;
        }
        else
        {
          tree[fromref] = tc;
          expr(0);
          exprassnvoid();
          mustbe(SEMICOLON, no_semicolon_in_for);
        }

        if (scaner() == SEMICOLON)  // cond
        {
          tree[condref] = 0;
        }
        else
        {
          tree[condref] = tc;
          exprval();
          sopnd--;
          mustbe(SEMICOLON, no_semicolon_in_for);
          sopnd--;
        }

        if (scaner() == RIGHTBR)  // incr
        {
          tree[incrref] = 0;
        }
        else
        {
          tree[incrref] = tc;
          expr(0);
          exprassnvoid();
          mustbe(RIGHTBR, no_rightbr_in_for);
        }

        flagsemicol = 0;
        tree[stmtref] = tc;
        inloop = 1;
        statement();
      }
        break;
      case LGOTO:
      {
        int fix = 1;
        int i;
        int flag = 1;

        mustbe(IDENT, no_ident_after_goto);
        totree(TGoto);

        for (i = 0; flag && i < pgotost - 1; i += 2)
        {
          flag = identab[gotost[i] + 1] != repr;
        }

        if (flag)
        {
          // первый раз встретился переход на метку, которой не было, в этом случае
          // ссылка на identtab, стоящая после TGoto, будет отрицательной
          int __tmp = -toidentab(1, 0);
          totree(__tmp);
          gotost[pgotost++] = lastid;
        }
        else
        {
          int _id = gotost[i - 2];

          if (gotost[_id + 1] < 0) // метка уже была
          {
            totree(_id);
            fix = 0;
          } else {
            totree(gotost[pgotost++] = _id);
          }
        }

        if (fix == 1)
          gotost[pgotost++] = line;
      }
        break;
      case LIF:
      {
        int elseref;

        totree(TIf);
        elseref = tc++;
        flagsemicol = 0;
        exprinbrkts(cond_must_be_in_brkts);
        sopnd--;
        statement();

        if (next == LELSE)
        {
          scaner();
          tree[elseref] = tc;
          statement();
        }
        else
        {
          tree[elseref] = 0;
        }
      }
        break;
      case LRETURN:
      {
        int ftype = modetab[functype + 1];

        wasret = 1;
        if (next == SEMICOLON)
        {
          if (ftype != LVOID)
          {
            error(no_ret_in_func);
          }
          totree(TReturnvoid);
        }
        else
        {
          if (ftype == LVOIDASTER)
          {
            flagsemicol = 0;
          }
          else
          {
            int szof_ftype = 0;
            if (ftype == LVOID)
            {
              error(notvoidret_in_void_func);
            }

            totree(TReturnval);
            szof_ftype = szof(ftype);
            totree(szof_ftype);
            scaner();
            expr(1);
            toval();
            sopnd--;

            if (ftype == LFLOAT && ansttype == LINT)
            {
              totree(WIDEN);
            }
            else if (ftype != ansttype)
            {
              error(bad_type_in_ret);
            }
            totree(TExprend);
          }
        }
      }
        break;
      case LSWITCH:
      {
        totree(TSwitch);
        exprinbrkts(cond_must_be_in_brkts);

        if (ansttype != LCHAR && ansttype != LINT)
        {
          error(float_in_switch);
        }

        sopnd--;
        scaner();
        inswitch = 1;
        block(-1);
        flagsemicol = 0;
        wasdefault = 0;
      }
        break;
      case LWHILE:
      {
        inloop = 1;
        totree(TWhile);
        flagsemicol = 0;
        exprinbrkts(cond_must_be_in_brkts);
        sopnd--;
        statement();
      }
        break;
      default:
      {
        expr(0);
        exprassnvoid();
      }
    }
  }

  if (flagsemicol && scaner() != SEMICOLON)
  {
    error(no_semicolon_after_stmt);
  }

  wasdefault = oldwasdefault;
  inswitch = oldinswitch;
  inloop = oldinloop;
}

int idorpnt(int e, int t)
{
  if (next == LMULT)
  {
    scaner();
    if (t == LVOID)
      t = LVOIDASTER;
    else
      t = newdecl(MPOINT, t);
    //t = t == LVOID ? LVOIDASTER : newdecl(MPOINT, t);
  }

  mustbe(IDENT, e);
  return t;
}

int struct_decl_list()
{
  int field_count = 0;
  int i;
  int t;
  int elem_type;
  int curdispl = 0;
  int wasarr = 0;
  int tstrbeg;
  int loc_modetab[100];
  int locmd = 3;

  loc_modetab[0] = MSTRUCT;
  tstrbeg = tc;
  totree(TStructbeg);
  tree[tc++] = 0;   // тут будет номер иниц процедуры

  scaner();
  scaner();

  do
  {
    int fieldrepr;

    int __type = gettype();

    t = elem_type = idorpnt(wait_ident_after_semicomma_in_struct, __type);
    fieldrepr = repr;

    if (next == LEFTSQBR)
    {
      int adN;
      int all;

      totree(TDeclarr);
      adN = tc++;
      t = arrdef(elem_type);  // Меняем тип (увеличиваем размерность массива)
      tree[adN] = arrdim;

      totree(TDeclid);
      totree(curdispl);
      totree(elem_type);
      totree(arrdim);           // N
      tree[all = tc++] = 0;       // all
      tree[tc++] = was_struct_with_arr; // proc
      totree(usual);            // usual
      totree(1);              // признак, что массив в структуре
      wasarr = 1;

      if (next == ASS)
      {
        scaner();
        scaner();

        if (is_array(t))    // инициализация массива
        {
          onlystrings = 2;
          tree[all] = 1;

          if (!usual)
          {
            tree[adN]--;  // это уменьшение N в Declarr
          }
          array_init(t);

          if (onlystrings == 1)
          {
            tree[all + 2] = usual + 2;  // только из строк 2 - без границ, 3 - с границами
          }
        }
        else
        {
          /*
            structdispl = identab[oldid+3];
            tree[all] = inition(t);
          */
        }
      }   // конец ASS
    }     // конец LEFTSQBR

    loc_modetab[locmd++] = t;
    loc_modetab[locmd++] = fieldrepr;
    field_count++;
    curdispl += szof(t);

    if (scaner() != SEMICOLON)
    {
      error(no_semicomma_in_struct);
    }
  } while (scaner() != END);

  if (wasarr)
  {
    totree(TStructend);
    totree(tstrbeg);
    tree[tstrbeg + 1] = was_struct_with_arr = procd++;
  }
  else
  {
    tree[tstrbeg] = NOP;
    tree[tstrbeg + 1] = NOP;
  }

  loc_modetab[1] = curdispl;      // тут длина структуры
  loc_modetab[2] = field_count * 2;

  modetab[md] = startmode;
  startmode = md++;
  for (i = 0; i < locmd; i++)
  {
    modetab[md++] = loc_modetab[i];
  }

  return check_duplicates();
}

int gettype()
{
  // gettype() выедает тип (кроме верхних массивов и указателей)
  // при этом, если такого типа нет в modetab, тип туда заносится;
  // возвращает отрицательное число(базовый тип), положительное (ссылка на modetab) или 0, если типа не было

  was_struct_with_arr = 0;
  if (is_int(type = cur) || is_float(type) || type == LVOID)
  {
    if (cur == LLONG)
      return LINT;
    else if (cur == LDOUBLE)
      return LFLOAT;
    else
      return type;
    //return (cur == LLONG ? LINT : cur == LDOUBLE ? LFLOAT : type);
  }
  else if (type == LSTRUCT)
  {
    if (next == BEGIN)    // struct {
    {
      return (struct_decl_list());
    }
    else if (next == IDENT)
    {
      int _l = reprtab[repr + 1];
      scaner();
      if (next == BEGIN)  // struct key {
      {
        // если такое описание уже было, то это ошибка - повторное описание
        int i;
        int lid;

        wasstructdef = 1; // это определение типа (может быть, без описания переменных)
        toidentab(1000, 0);
        lid = lastid;
        identab[lid + 2] = struct_decl_list();
        identab[lid + 3] = 1000 + was_struct_with_arr;

        return identab[lid + 2];
      }
      else        // struct key это применение типа
      {
        if (_l == 1)
        {
          error(ident_is_not_declared);
        }

        was_struct_with_arr = identab[_l + 3] - 1000;
        return (identab[_l + 2]);
      }
    }
    else
    {
      error(wrong_struct);
    }
  }
  else if (cur == IDENT)
  {
    applid();

    if (identab[lastid + 3] < 1000)
    {
      error(ident_not_type);
    }

    was_struct_with_arr = identab[lastid + 3] - 1000;
    return identab[lastid + 2];
  }
  else
  {
    error(not_decl);
  }
  return 0;
}

void block(int b)
{
  // если   b ==  1  - то это просто блок,
  //        b ==  2  - блок нити,
  //        b == -1  - блок в switch,
  // иначе (b ==  0) - это блок функции

  int oldinswitch = inswitch;
  int notended = 1;
  int i;
  int olddispl;
  int oldlg = lg;
  int _firstdecl;

  inswitch = b < 0;
  totree(TBegin);

  if (b)
  {
    olddispl = displ;
    curid = id;
  }
  blockflag = 0;

  while (is_int(next) || is_float(next) || next == LSTRUCT || next == LVOID)
  {
    int repeat = 1;

    scaner();
    _firstdecl = gettype();

    if (wasstructdef && next == SEMICOLON)
    {
      scaner();
      continue;
    }

    do
    {
      int _idorpnt = idorpnt(after_type_must_be_ident, _firstdecl);
      decl_id(_idorpnt);

      if (next == COMMA)
      {
        scaner();
      }
      else if (next == SEMICOLON)
      {
        scaner();
        repeat = 0;
      }
      else
      {
        error(def_must_end_with_semicomma);
      }
    } while (repeat);
  }

  // кончились описания, пошли операторы до }

  do
  {
    if (b == 2 && next == _TEXIT || b != 2 && next == END)
    //if (b == 2 ? next == _TEXIT : next == END)
    {
      scaner();
      notended = 0;
    }
    else
    {
      statement();
    }
  } while (notended);

  if (b)
  {
    for (i = id - 4; i >= curid; i -= 4)
    {
      reprtab[identab[i + 1] + 1] = identab[i];
    }
    displ = olddispl;
  }
  inswitch = oldinswitch;
  lg = oldlg;
  totree(TEnd);
}

void function_definition()
{
  int fn = identab[lastid + 3];
  int i;
  int pred;
  int oldrepr = repr;
  int ftype;
  int n;
  int fid = lastid;
  int olddispl = displ;

  pgotost = 0;
  functype = identab[lastid + 2];
  ftype = modetab[functype + 1];
  n = modetab[functype + 2];
  wasret = 0;
  displ = 3;
  maxdispl = 3;
  lg = 1;

  if ((pred = identab[lastid]) > 1)     // был прототип
  {
    if (functype != identab[pred + 2])
    {
      error(decl_and_def_have_diff_type);
    }
    identab[pred + 3] = fn;
  }
  curid = id;

  for (i = 0; i < n; i++)
  {
    type = modetab[functype + i + 3];
    repr = functions[fn + i + 1];
    if (repr > 0)
    {
      toidentab(0, type);
    }
    else
    {
      repr = -repr;
      toidentab(-1, type);
    }
  }

  functions[fn] = tc;
  totree(TFuncdef);
  totree(fid);
  pred = tc++;
  repr = oldrepr;

  block(0);

  // if (ftype == LVOID && tree[tc - 1] != TReturnvoid)
  // {
  tc--;
  totree(TReturnvoid);
  totree(TEnd);
  // }

  if (ftype != LVOID && !wasret)
  {
    error(no_ret_in_func);
  }

  for (i = id - 4; i >= curid; i -= 4)
  {
    reprtab[identab[i + 1] + 1] = identab[i];
  }

  for (i = 0; i < pgotost - 1; i += 2)
  {
    repr = identab[gotost[i] + 1];
    hash = gotost[i + 1];

    if (hash < 0)
    {
      hash = -hash;
    }

    if (!identab[gotost[i] + 2])
    {
      error(label_not_declared);
    }
  }

  curid = 2;        // все функции описываются на одном уровне
  tree[pred] = maxdispl;  // + 1; ?
  lg = -1;
  displ = olddispl;
}

int func_declarator(int level, int func_d, int _firstdecl)
{
  // на 1 уровне это может быть определением функции или предописанием, на остальных уровнях
  // - только декларатором (без идентов)

  int loc_modetab[100];
  int locmd;
  int numpar = 0;
  int ident;
  int maybe_fun;
  int repeat = 1;
  int i;
  int wastype = 0;
  int old;

  loc_modetab[0] = MFUNCTION;
  loc_modetab[1] = _firstdecl;
  loc_modetab[2] = 0;
  locmd = 3;

  while (repeat)
  {
    if (cur == LVOID || is_int(cur) || is_float(cur) || cur == LSTRUCT)
    {
      maybe_fun = 0;  // м.б. параметр-ф-ция?
              // 0 - ничего не было,
              // 1 - была *,
              // 2 - была [

      ident = 0;    // = 0 - не было идента,
              // 1 - был статический идент,
              // 2 - был идент-параметр-функция

      wastype = 1;
      type = gettype();

      if (next == LMULT)
      {
        maybe_fun = 1;
        scaner();
        if (type == LVOID)
          type = LVOIDASTER;
        else 
          type = newdecl(MPOINT, type);
        //type = type == LVOID ? LVOIDASTER : newdecl(MPOINT, type);
      }

      if (level)
      {
        if (next == IDENT)
        {
          scaner();
          ident = 1;
          functions[funcnum++] = repr;
        }
      }
      else if (next == IDENT)
      {
        error(ident_in_declarator);
      }

      if (next == LEFTSQBR)
      {
        maybe_fun = 2;

        if (is_pointer(type) && ident == 0)
        {
          error(aster_with_row);
        }

        while (next == LEFTSQBR)
        {
          scaner();
          mustbe(RIGHTSQBR, wait_right_sq_br);
          type = newdecl(MARRAY, type);
        }
      }
    }

    if (cur == LVOID)
    {
      type = LVOID;
      wastype = 1;

      if (next != LEFTBR)
      {
        error(par_type_void_with_nofun);
      }
    }

    if (wastype)
    {
      numpar++;
      loc_modetab[locmd++] = type;

      if (next == LEFTBR)
      {
        scaner();
        mustbe(LMULT, wrong_fun_as_param);

        if (next == IDENT)
        {
          if (level)
          {
            scaner();

            if (ident == 0)
            {
              ident = 2;
            }
            else
            {
              error(two_idents_for_1_declarer);
            }
            functions[funcnum++] = -repr;
          }
          else
          {
            error(ident_in_declarator);
          }
        }

        mustbe(RIGHTBR, no_right_br_in_paramfun);
        mustbe(LEFTBR, wrong_fun_as_param);
        scaner();

        if (maybe_fun == 1)
        {
          error(aster_before_func);
        }
        else if (maybe_fun == 2)
        {
          error(array_before_func);
        }

        old = func_def;
        loc_modetab[locmd - 1] = func_declarator(0, 2, type);
        func_def = old;
      }

      if (func_d == 3)
      {
        if (ident > 0)
          func_d = 1;
        else
          func_d = 2;
        //func_d = ident > 0 ? 1 : 2;
      }
      else if (func_d == 2 && ident > 0)
      {
        error(wait_declarator);
      }
      else if (func_d == 1 && ident == 0)
      {
        error(wait_definition);
      }

      if (scaner() == COMMA)
      {
        scaner();
      }
      else if (cur == RIGHTBR)
      {
        repeat = 0;
      }
    }
    else if (cur == RIGHTBR)
    {
      repeat = 0;
      func_d = 0;
    }
    else
    {
      error(wrong_param_list);
    }
  }
  func_def = func_d;
  loc_modetab[2] = numpar;

  modetab[md] = startmode;
  startmode = md++;
  for (i = 0; i < numpar + 3; i++)
  {
    modetab[md++] = loc_modetab[i];
  }

  return check_duplicates();
}

void ext_decl()
{
  int i;
  do    // top level описания переменных и функций до конца файла
  {
    int repeat = 1;
    int funrepr;
    int _first = 1;

    wasstructdef = 0;
    scaner();

    /*
      if (cur == SH_DEFINE)
      {
        mustbe(IDENT, no_ident_in_define);

        if (scaner() == LMINUS)
        {
          scaner(), k = -1;
        }

        if (cur != NUMBER || ansttype != LINT)
        {
          error(not_int_in_define);
        }

        toidentab(-2, k * num);
        continue;
      }
    */

    firstdecl = gettype();
    if (wasstructdef && next == SEMICOLON)  // struct point {float x, y;};
    {
      scaner();
      continue;
    }

    func_def = 3; // func_def = 0 - (),
            // 1 - определение функции,
            // 2 - это предописание,
            // 3 - не знаем или вообще не функция

    /*
      if (firstdecl == 0)
      {
        firstdecl = LINT;
      }
    */

    do  // описываемые объекты через ',' определение функции может быть только одно, никаких ','
    {
      int _ex = 0;
      type = firstdecl;
      if (next == LMULT)
      {
        scaner();
        if (firstdecl == LVOID)
          type = LVOIDASTER;
        else
          type = newdecl(MPOINT, firstdecl);
        //type = firstdecl == LVOID ? LVOIDASTER : newdecl(MPOINT, firstdecl);
      }
      mustbe(IDENT, after_type_must_be_ident);

      if (next == LEFTBR) // определение или предописание функции
      {
        int oldfuncnum = funcnum++;
        int firsttype = type;

        funrepr = repr;
        scaner();
        scaner();
        type = func_declarator(_first, 3, firsttype);  // выкушает все параметры до ) включительно

        if (next == BEGIN)
        {
          if (func_def == 0)
          {
            func_def = 1;
          }
        }
        else if (func_def == 0)
        {
          func_def = 2;
        }
        // теперь я точно знаю, это определение ф-ции или предописание (func_def = 1 или 2)
        repr = funrepr;

        toidentab(oldfuncnum, type);

        if (next == BEGIN)
        {
          scaner();
          if (func_def == 2)
          {
            error(func_decl_req_params);
          }

          function_definition();
          _ex = 1;
        }
        else
        {
          if (func_def == 1)
          {
            error(function_has_no_body);
          }
        }
      }
      else if (firstdecl == LVOID)
      {
        error(only_functions_may_have_type_VOID);
      }
      if (_ex)
        break;
      // описания идентов-не-функций

      if (func_def == 3)
      {
        decl_id(type);
      }

      if (next == COMMA)
      {
        scaner();
        _first = 0;
      }
      else if (next == SEMICOLON)
      {
        scaner();
        repeat = 0;
      }
      else
      {
        error(def_must_end_with_semicomma);
      }
    } while (repeat);

  } while (next != LEOF);

  if (wasmain == 0)
  {
    error(no_main_in_program);
  }

  for (i = 0; i <= prdf; i++)
  {
    if (predef[i])
    {
      repr = predef[i];
      error(predef_but_notdef);
    }
  }

  totree(TEnd);
}

#endif 


