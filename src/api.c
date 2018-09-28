#include "api.h"

_Noreturn void freeallexit(buf* dt, _Bool code)
{
	free(dt->fname);
	for(buf_t ln = 0; ln <= dt->lns; ln++)
	{
		free(dt->txt[ln]);
	}
	free(dt->txt);
	free(dt);
	exit(code);
}

buf* freeblk(buf* dt)
{
	_Bool line_back = 0;
	if(dt->chrs_ln - 1 % MEMBLK == MEMBLK - 1)
	{
		CURRLN = realloc(CURRLN, (2 * dt->chrs_ln) - MEMBLK);
		checkptr(dt, CURRLN, "free the memblock with chars\0");
	}
	if(dt->chrs_ln > 0)
	{
		dt->chrs_ln--;
		dt->chrs--;
	}
	else if(dt->chrs_ln == 0)
	{
		line_back = 1;
		dt->cusr_y = 0;
	}
	CURRLN[dt->chrs_ln] = NTERM;

	if(line_back == 1 && dt->lns > 0 && UPLN[strlen(UPLN) - NTERM_SZ] == LF)
	{
		free(CURRLN);
		dt->lns--;
		dt->chrs_ln = (buf_t) strlen(CURRLN) - NTERM_SZ;
		CURRLN[dt->chrs_ln] = NTERM;
		if(dt->chrs > 0) // Just for the LF removal.
		{
			dt->chrs--;
		}
	}
	return dt;
}

buf* allocblk(buf* dt, char mode)
{
	switch(mode)
	{
		case 'c':
			dt->chrs++;
			dt->chrs_ln++;
			if(dt->chrs_ln % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				CURRLN = realloc(CURRLN, dt->chrs_ln + MEMBLK);
				checkptr(dt, CURRLN, "alloc new memblock for chars\0");
			}
			break;

		case 'l':
			if(dt->lns++ >= MAX_LNS)
			{
				dt->lns = MAX_LNS;
			}
			else
			{
				if(dt->lns % MEMBLK == 0) // Allocates with a one line reserve.
				{
					dt->txt =
					realloc(dt->txt, sizeof(dt->txt) * (dt->lns + MEMBLK));
				}
				CURRLN = malloc(MEMBLK);
				checkptr(dt, CURRLN, "alloc byte in the new line\0");
			}
			CURRLN[dt->chrs_ln = 0] = NTERM;
			break;
	}
	return dt;
}

buf* txtshift(buf* dt)
{
	for(term_t x = dt->chrs_ln; x >= dt->chrs_ln - dt->cusr_x; x--)
	{
		CURRLN[x] = CURRLN[x - 1];
	}
	return dt;
}

buf* charadd(buf* dt, char key)
{
	if(dt->chrs <= MAX_CHRS)
	{
		dt = allocblk(dt, 'c');

		if(dt->cusr_x > 0)
		{
			for(term_t x = dt->chrs_ln; x >= dt->chrs_ln - dt->cusr_x; x--)
			{
				CURRLN[x] = CURRLN[x - 1];
			}
		}
		dt->txt[dt->lns - dt->cusr_y][LASTCHR - dt->cusr_x] = key;
		dt->txt[dt->lns - dt->cusr_y][dt->chrs_ln] = NTERM;
		if(key == NTERM && dt->chrs > 0) // Initializer.
		{
			dt->chrs--;
			dt->chrs_ln--;
		}
		switch(key)
		{
			case TAB:
				CURRLN[LASTCHR] = ' '; // Currently onverts TAB to SPACE.
				break;

			case LF: // TODO: WHEN ENTER.
				dt->cusr_y = 0;
				dt = allocblk(dt, 'l');
				if(dt->cusr_x > 0)
				{
					for(term_t x = dt->chrs_ln; x >= dt->chrs_ln - dt->cusr_x; x--)
					{
						CURRLN[x] = CURRLN[x - 1];
						strcpy(&dt->txt[dt->lns][x], &dt->txt[dt->lns - 1][x]);
					}
				}
				break;
		}
	}
	return dt;
}

buf* recochar(buf* dt, char key) // TODO: KEYMAP.
{
	if(key != ESCAPE)
	{
		switch(key)
		{
			case NEG: // Pipe and signal prevention. TODO: FULL UPPER BAR FLUSH.
				freeallexit(dt, 0);

			case CTRL_D:
				savefile(dt);
				break;

			case CTRL_H:
				if(dt->cusr_x > 0)
				{
					dt->cusr_x--;
				}
				break;

			case CTRL_G:
				if(dt->cusr_x < dt->chrs_ln)
				{
					dt->cusr_x++;
				}
				break;

			case CTRL_Y:
				if(dt->cusr_y < dt->lns)
				{
					dt->cusr_y++;
				}
				break;

			case CTRL_B:
				if(dt->cusr_y > 0)
				{
					dt->cusr_y--;
				}
				break;

			case BACKSPACE:
				dt = freeblk(dt);
				break;

			default:
				dt = charadd(dt, key);
				break;
		}
	}
	printf("cusr_x: %d cusr_y: %d\n", dt->cusr_x, dt->cusr_y); // DEBUG
	return dt;
}

