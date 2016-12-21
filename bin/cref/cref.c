/* cref.c - generate cross reference */
/* Author: M. Brent Harp    (brharp@uoguelph.ca) */
/* Date: 12/20/2016 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define    BUFSIZE     100

char buf[BUFSIZE];		/* buffer for ungetch */
int bufp = 0;			/* next free position in buf */

getch ()			/* get a (possible unread) character */
{
  return ((bufp > 0) ? buf[--bufp] : getchar ());
}

ungetch (c)			/* push character back on input */
     int c;
{
  if (bufp > BUFSIZE)
    printf ("ungetch: too many characters\n");
  else
    buf[bufp++] = c;
}


#define	LETTER	'a'
#define	DIGIT	'0'

type (c)			/* return type of ASCII character */
     int c;
{
  if (isalpha (c))
    return (LETTER);
  else if (isdigit (c))
    return (DIGIT);
  else
    return (c);
}

getword (w, lim)		/* get next word from input */
     char *w;
     int lim;
{
  int c, t;

  if (type (c = *w++ = getch ()) != LETTER)
    {
      *w = '\0';
      return (c);
    }
  while (--lim > 0)
    {
      t = type (c = *w++ = getch ());
      if (t != LETTER && t != DIGIT)
	{
	  ungetch (c);
	  break;
	}
    }
  *(w - 1) = '\0';
  return (LETTER);
}


#define	MAXWORD	20

struct tnode
{				/* the basic node */
  char *word;			/* text */
  int line;			/* line number */
  int count;
  struct tnode *left;		/* left child */
  struct tnode *right;		/* right child */
}


treeprint (p)			/* print tree p */
     struct tnode *p;
{
  static char *lastword = NULL;
  static int lastline = -1;

  if (p != NULL)
    {
      treeprint (p->left);
      if (lastword == NULL || strcmp (p->word, lastword) != 0)
	{
	  if (lastword != NULL)
	    printf ("\n");
	  lastword = p->word;
	  lastline = -1;
	  printf ("%s", p->word);
	}
      if (lastline != p->line)
	{
	  lastline = p->line;
	  printf (" %d", p->line);
	}
      treeprint (p->right);
    }
}


main ()				/* cross referencer */
{
  struct tnode *root, *tree ();
  char word[MAXWORD];
  int t, lineno = 1;

  root = NULL;
  while ((t = getword (word, MAXWORD)) != EOF)
    if (t == LETTER)
      root = tree (root, word, lineno);
    else if (t == '\n')
      ++lineno;
  treeprint (root);
  printf ("\n");
}

struct tnode *
tree (p, w, n)			/* insert (w, n) at or below p */
     struct tnode *p;
     char *w;
     int n;
{
  struct tnode *talloc ();
  int cond, tcmp ();

  if (p == NULL)
    {				/* new word */
      p = talloc ();		/* make a new node */
      p->word = strdup (w);
      p->line = n;
      p->count = 1;
      p->left = p->right = NULL;
    }
  else if ((cond = tcmp (w, n, p)) == 0)
    p->count++;			/* repeat word */
  else if (cond < 0)		/* lower goes left */
    p->left = tree (p->left, w, n);
  else				/* greater goes right */
    p->right = tree (p->right, w, n);
  return (p);
}


tcmp (w, n, p)			/* compare (w,n) with p */
     char *w;
     int n;
     struct tnode *p;
{
  int comp;
  if ((comp = strcmp (w, p->word)) == 0)
    return (n - p->line);
  else
    return (comp);
}

struct tnode *
talloc ()
{
  return ((struct tnode *) malloc (sizeof (struct tnode)));
}
