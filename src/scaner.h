#ifndef H_SCANER
#define H_SCANER 1

#include "defs.h"
#include "errors.h"
#include "global.h"
#include "stdio.h"
#include "utils.h"


int getnext();
void nextch();

int letter();
int digit();
int equal(int, int);

int scan();
int scaner();

int getnext()
{
    // reads UTF-8

    char firstchar;
    char secondchar;

    firstchar = fgetc(input);
    if (firstchar == EOF)
    {
        return EOF;
    }
    else
    {
        nextchar = firstchar;

        if (nextchar == 13 /* cr */)
        {
            getnext();
        }
    }

    return nextchar;
}

void onemore()
{
    curchar = nextchar;
    nextchar = getnext();
}

void endofline()
{
    if (prep_flag == 1)
    {
        int j;

        printf("line %i) ", line - 1);

        for (j = lines[line - 1]; j < lines[line]; j++)
        {
            if (source[j] != EOF)
            {
                printf_char(source[j]);
            }
        }
        // реализовано на VM
        //fflush(stdout);
    }
}

void endnl()
{
    lines[++line] = charnum;
    lines[line + 1] = charnum;

    if (kw)
    {
        endofline();
    }
}

void nextch()
{
    onemore();
    if (curchar == EOF)
    {
        onemore();
        lines[++line] = charnum;
        lines[line + 1] = charnum;

        if (kw)
        {
            endofline();
            printf("\n");
        }

        return;
    }

    source[charnum++] = curchar;
    if (instring)
    {
        return;
    }

    if (curchar == '/' && nextchar == '/')
    {
        do
        {
            onemore();
            source[charnum++] = curchar;

            if (curchar == EOF)
            {
                endnl();
                printf("\n");
                return;
            }
        } while (curchar != '\n');

        endnl();
        return;
    }

    if (curchar == '/' && nextchar == '*')
    {
        onemore();
        source[charnum++] = curchar;	// надо сразу выесть /*, чтобы не попасть на /*/
        do
        {
            onemore();
            source[charnum++] = curchar;

            if (curchar == EOF)
            {
                endnl();
                printf("\n");
                error(comm_not_ended);
            }

            if (curchar == '\n')
            {
                endnl();
            }
        } while (curchar != '*' || nextchar != '/');

        onemore();
        source[charnum++] = curchar;
        curchar = ' ';

        return;
    }

    if (curchar == '\n')
    {
        endnl();
    }

    return;
}

void next_string_elem()
{
    num = curchar;
    if (curchar == '\\')
    {
        nextch();

        if (curchar == 'n' || curchar == 'н' /*'н'*/)
        {
            num = 10;
        }
        else if (curchar == 't' || curchar == 'т' /*'т'*/)
        {
            num = 9;
        }
        else if (curchar == '0')
        {
            num = 0;
        }
        else if (curchar != '\'' && curchar != '\\' && curchar != '\"')
        {
            error(bad_escape_sym);
        }
        else
        {
            num = curchar;
        }
    }

    nextch();
}

int letter()
{
    return (curchar >= 'A' && curchar <= 'Z') || (curchar >= 'a' && curchar <= 'z') || curchar == '_' ||
           (curchar >= 'А' /*'А'*/ && curchar <= 'я' /*'я'*/);
}

int digit()
{
    return curchar >= '0' && curchar <= '9';
}

int ispower()
{
	return curchar == 'e' || curchar == 'E';	// || curchar == 'е' || curchar == 'Е')	// это русские е и Е
}

int equal(int i, int j)
{
	++i;
	++j;

	while (reprtab[++i] == reprtab[++j])
	{
		if (reprtab[i] == 0 && reprtab[j] == 0)
		{
			return 1;
		}
	}

	return 0;
}

int scan()
{
	int cr;

	while (curchar == ' ' || curchar == '\t' || curchar == '\n')
	{
		nextch();
	}

	// printf("scan curchar=%c %i\n", curchar, curchar);
	switch (curchar)
	{
		case EOF:
			return LEOF;
		case ',':
		{
			nextch();
			return COMMA;
		}

		case '=':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = EQEQ;
			}
			else
			{
				cr = ASS;
			}

			return cr;
		}

		case '*':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = MULTASS;
			}
			else
			{
				cr = LMULT;
			}

			return cr;
		}

		case '/':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = DIVASS;
			}
			else
			{
				cr = LDIV;
			}

			return cr;
		}

		case '%':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = REMASS;
			}
			else
			{
				cr = LREM;
			}

			return cr;
		}

		case '+':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = PLUSASS;
			}
			else if (curchar == '+')
			{
				nextch();
				cr = INC;
			}
			else
			{
				cr = LPLUS;
			}

			return cr;
		}

		case '-':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = MINUSASS;
			}
			else if (curchar == '-')
			{
				nextch();
				cr = DEC;
			}
			else if (curchar == '>')
			{
				nextch();
				cr = ARROW;
			}
			else
			{
				cr = LMINUS;
			}

			return cr;
		}

		case '<':
		{
			nextch();

			if (curchar == '<')
			{
				nextch();
				if (curchar == '=')
				{
					nextch();
					cr = SHLASS;
				}
				else
				{
					cr = LSHL;
				}
			}
			else if (curchar == '=')
			{
				nextch();
				cr = LLE;
			}
			else
			{
				cr = LLT;
			}

			return cr;
		}

		case '>':
		{
			nextch();

			if (curchar == '>')
			{
				nextch();

				if (curchar == '=')
				{
					nextch();
					cr = SHRASS;
				}
				else
				{
					cr = LSHR;
				}
			}
			else if (curchar == '=')
			{
				nextch();
				cr = LGE;
			}
			else
			{
				cr = LGT;
			}

			return cr;
		}

		case '&':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = ANDASS;
			}
			else if (curchar == '&')
			{
				nextch();
				cr = LOGAND;
			}
			else
			{
				cr = LAND;
			}

			return cr;
		}

		case '^':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = EXORASS;
			}
			else
			{
				cr = LEXOR;
			}

			return cr;
		}

		case '|':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = ORASS;
			}
			else if (curchar == '|')
			{
				nextch();
				cr = LOGOR;
			}
			else
			{
				cr = LOR;
			}

			return cr;
		}

		case '!':
		{
			nextch();

			if (curchar == '=')
			{
				nextch();
				cr = NOTEQ;
			}
			else
			{
				cr = LOGNOT;
			}

			return cr;
		}

		case '\'':
		{
			instring = 1;
			nextch();
			next_string_elem();

			if (curchar != '\'')
			{
				error(no_right_apost);
			}

			nextch();
			instring = 0;
			ansttype = LCHAR;

			return NUMBER;
		}

		case '\"':
		{
			int n = 0;
			int flag = 1;

			instring = 1;
			nextch();
			while (flag)
			{
				while (curchar != '\"' && n < MAXSTRINGL)
				{
					next_string_elem();
					lexstr[n++] = num;
					// printf("n= %i %c %i\n", n-1, num, num);
				}

				if (n == MAXSTRINGL)
				{
					error(too_long_string);
				}

				nextch();
				while (curchar == ' ' || curchar == '\t' || curchar == '\n')
				{
					nextch();
				}

				if (curchar == '\"')
				{
					nextch();
				}
				else
				{
					flag = 0;
				}
			}

			num = n;
			instring = 0;
			return STRING;
		}

		case '(':
		{
			nextch();
			return LEFTBR;
		}

		case ')':
		{
			nextch();
			return RIGHTBR;
		}

		case '[':
		{
			nextch();
			return LEFTSQBR;
		}

		case ']':
		{
			nextch();
			return RIGHTSQBR;
		}

		case '~':
		{
			nextch();
			return LNOT;	// поразрядное отрицание
		}
		case '{':
		{
			nextch();
			return BEGIN;
		}
		case '}':
		{
			nextch();
			return END;
		}
		case ';':
		{
			nextch();
			return SEMICOLON;
		}
		case '?':
		{
			nextch();
			return QUEST;
		}
		case ':':
		{
			nextch();
			return COLON;
		}
		case '.':
			if (nextchar < '0' || nextchar > '9')
			{
				nextch();
				return DOT;
			}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
	   {
      int flagint = 1;
      int flagtoolong = 0;
      double k;

      num = 0;
      numdouble = 0.0;
      while (digit())
      {
        numdouble = numdouble * 10 + (curchar - '0');
        if (numdouble > INT_MAX)
        {
          flagtoolong = 1;
          flagint = 0;
        }

        num = num * 10 + (curchar - '0');
        nextch();
      }

			if (curchar == '.')
			{
				flagint = 0;
				nextch();
				k = 0.1;

				while (digit())
				{
					numdouble += (curchar - '0') * k;
					k *= 0.1;
					nextch();
				}
			}

			if (ispower())
			{
				int d = 0;
				int k = 1;
				int i;

				nextch();
				if (curchar == '-')
				{
					flagint = 0;
					nextch();
					k = -1;
				}
				else if (curchar == '+')
				{
					nextch();
				}

				if (!digit())
				{
					error(must_be_digit_after_exp);
				}

				while (digit())
				{
					d = d * 10 + curchar - '0';
					nextch();
				}

				if (flagint)
				{
					for (i = 1; i <= d; i++)
					{
						num *= 10;
					}
				}

				if (k > 0)
				{
					for (i = 1; i <= d; i++)
					{
						numdouble *= 10;
					}
				}
				else
				{
					for (i = 1; i <= d; i++)
					{
						numdouble *= 0.1;
					}
				}
			}

			if (flagint)
			{
				ansttype = LINT;
				return NUMBER;
			}
			else
			{
				if (flagtoolong)
				{
					warning(too_long_int);
				}
				ansttype = LFLOAT;
			}

			dtonumr(&numr, &numdouble);
			return NUMBER;
		}


    default:
    {
      if (letter() || curchar == 43)
      {
        int oldrepr = rp;
        int r;
        rp += 2;
        hash = 0;

        // решетка на 1 месте -- значит, ключевое слово препроцессора
        do
        {
          hash += curchar;
          reprtab[rp++] = curchar;
          nextch();
        } while (letter() || digit());

        hash &= 255;
        reprtab[rp++] = 0;
        r = hashtab[hash];
        if (r)
        {
          do
          {
            if (equal(r, oldrepr))
            {
              rp = oldrepr;
              return (reprtab[r + 1] < 0) ? reprtab[r + 1] : (repr = r, IDENT);
            }
            else
            {
              r = reprtab[r];
            }
          } while (r);
        }

        reprtab[oldrepr] = hashtab[hash];
        repr = hashtab[hash] = oldrepr;
        reprtab[repr + 1] = (keywordsnum) ? -((++keywordsnum - 2) / 4) : 1;
          // == 0 - только MAIN,
          // <  0 - ключевые слова,
          // == 1 - обычные иденты

        return IDENT;
      }
      else
      {
        printf("плохой символ %i\n", curchar);
        nextch();
        t_exit();
      }
    }
  }
}

int scaner()
{
	cur = next;
	next = scan();

	/*
		if(kw)
		{
			printf("scaner cur %i next %i repr %i\n", cur, next, repr);
		}
	*/

	return cur;
}

#endif
