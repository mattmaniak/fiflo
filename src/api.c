#include "api.h"

buf* freeblk(buf* dt)
{
	_Bool line_back = 0;
	if(dt->chrs_ln - 1 % MEMBLK == MEMBLK - 1)
	{
		CURRLN = realloc(CURRLN, (2 * dt->chrs_ln) - MEMBLK);
		checkptr(dt, CURRLN, "free the memblock with a line\0");
	}
	if(dt->chrs_ln > 0 && dt->cusr_x != (dt->chrs_ln + INDEX))
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

buf* txtshift(buf* dt, char direction)
{
	switch(direction)
	{
		case 'l':
			if(dt->cusr_x > dt->chrs_ln - 1 && dt->chrs_ln > 0)
			{
				dt->cusr_x = dt->chrs_ln - 1;
			}
			if(dt->chrs_ln > 0)
			{
				for(term_t x = dt->chrs_ln - dt->cusr_x; x <= dt->chrs_ln; x++)
				{
					CURRLN[x - 1] = CURRLN[x];
				}
			}
			break;

		case 'r':
			for(term_t x = dt->chrs_ln; x >= dt->chrs_ln - dt->cusr_x; x--)
			{
				CURRLN[x] = CURRLN[x - 1];
			}
			break;
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
			txtshift(dt, 'r');
		}
		dt->txt[dt->lns - dt->cusr_y][LASTCHR - dt->cusr_x] = key;
		dt->txt[dt->lns - dt->cusr_y][dt->chrs_ln] = NTERM;

		if(key == NTERM && dt->chrs > 0) // Initializer.
		{
			dt->chrs--;
			dt->chrs_ln--;
		}
		switch(key) // TODO: TAB
		{
			case LF: // TODO: WHEN ENTER.
				dt = allocblk(dt, 'l');
				if(dt->cusr_x > 0)
				{
/*					for(term_t x = strlen(UPLN) - dt->cusr_x; x <= strlen(UPLN); x++)
					{
						CURRLN[dt->chrs_ln++] = UPLN[x];
					}
					UPLN[strlen(UPLN) - dt->cusr_x] = NTERM;
					dt->cusr_x = dt->chrs_ln;
*/				}
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
					if(strlen(UPLN) == 1) // Must contain at least newline.
					{
						dt->chrs = 1;
					}
					else
					{
						dt->chrs_ln = strlen(UPLN) - INDEX;
					}
				}
				break;

			case CTRL_B:
				if(dt->cusr_y > 0)
				{
					dt->cusr_y--;
				}
				break;

			case BACKSPACE:
				if(dt->cusr_x != dt->chrs_ln) // Left side protection.
				{
					dt = txtshift(dt, 'l');
					dt = freeblk(dt);
				}
				break;

			default:
				dt = charadd(dt, key);
				break;
		}
	}
	printf("cusr_x: %d cusr_y: %d\n", dt->cusr_x, dt->cusr_y); // DEBUG
	return dt;
}

