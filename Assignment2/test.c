#include "ansidecl.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include "str.h"

#ifndef HAVE_GNU_LD
// #define _sys_nerr	sys_nerr
// #define _sys_errlist	sys_errlist
#endif

#define	STREQ(a, b)	(StrCompare((a), (b)) == 0)

CONST char *it = "<UNSET>";	/* Routine name for message routines. */
size_t errors = 0;

/* Complain if condition is not true.  */
void
DEFUN(check, (thing, number), int thing AND int number)
{
  if (!thing)
    {
      printf("%s flunked test %d\n", it, number);
      ++errors;
    }
}

/* Complain if first two args don't StrCompare as equal.  */
void
DEFUN(equal, (a, b, number), CONST char *a AND CONST char *b AND int number)
{
  check(a != NULL && b != NULL && STREQ(a, b), number);
}

char one[50];
char two[50];

int
DEFUN(main, (argc, argv), int argc AND char **argv)
{
  /* Test StrCompare first because we use it to test other things.  */
  it = "StrCompare";
  check(StrCompare("", "") == 0, 1);		/* Trivial case. */
  check(StrCompare("a", "a") == 0, 2);		/* Identity. */
  check(StrCompare("abc", "abc") == 0, 3);		/* Multicharacter. */
  check(StrCompare("abc", "abcd") < 0, 4);		/* Length mismatches. */
  check(StrCompare("abcd", "abc") > 0, 5);
  check(StrCompare("abcd", "abce") < 0, 6);		/* Honest miscompares. */
  check(StrCompare("abce", "abcd") > 0, 7);
  check(StrCompare("a\203", "a") > 0, 8);		/* Tricky if char signed. */
  check(StrCompare("a\203", "a\003") > 0, 9);

  it = "StrCaseCompare";
  check(StrCaseCompare("", "") == 0, 1);		/* Trivial case. */
  check(StrCaseCompare("a", "A") == 0, 2);		/* Identity. */
  check(StrCaseCompare("A", "A") == 0, 3);		/* Identity. */
  check(StrCaseCompare("a", "a") == 0, 4);		/* Identity. */
  
  check(StrCaseCompare("abc", "abc") == 0, 5);		/* Multicharacter. */
  check(StrCaseCompare("abc", "abC") == 0, 6);		/* Multicharacter. */
  check(StrCaseCompare("aBc", "abC") == 0, 7);		/* Multicharacter. */
  check(StrCaseCompare("abc", "abcd") < 0, 8);		/* Length mismatches. */
  check(StrCaseCompare("abcd", "abc") > 0, 9);
  check(StrCaseCompare("abcd", "abce") < 0, 10);		/* Honest miscompares. */
  check(StrCaseCompare("abce", "abcd") > 0, 11);
  check(StrCaseCompare("a\203", "a") > 0, 12);		/* Tricky if char signed. */
  check(StrCaseCompare("a\203", "a\003") > 0, 13);
  check(StrCaseCompare("a\203", "A") > 0, 14);		/* Tricky if char signed. */
  check(StrCaseCompare("a\203", "a\003") > 0, 15);
  check(StrCaseCompare("a", "b") < 0, 16);		/* Identity. */
  check(StrCaseCompare("a", "B") < 0, 17);		/* Identity. */
  check(StrCaseCompare("A", "b") < 0, 18);		/* Identity. */
  check(StrCaseCompare("B", "A") > 0, 19);		/* Identity. */
  /* Test strcpy next because we need it to set up other tests.  */
  it = "StrCopy";
  check(StrCopy(one, "abcd") == one, 1);	/* Returned value. */
  equal(one, "abcd", 2);		/* Basic test. */

  (void) StrCopy(one, "x");
  equal(one, "x", 3);			/* Writeover. */
  equal(one+2, "cd", 4);		/* Wrote too much? */

  (void) StrCopy(two, "hi there");
  (void) StrCopy(one, two);
  equal(one, "hi there", 5);		/* Basic test encore. */
  equal(two, "hi there", 6);		/* Stomped on source? */

  (void) StrCopy(one, "");
  equal(one, "", 7);			/* Boundary condition. */

  /* StrConcat.  */
  it = "StrConcat";
  (void) strcpy(one, "ijk");
  check(StrConcat(one, "lmn") == one, 1);	/* Returned value. */
  equal(one, "ijklmn", 2);		/* Basic test. */

  (void) strcpy(one, "x");
  (void) StrConcat(one, "yz");
  equal(one, "xyz", 3);			/* Writeover. */
  equal(one+4, "mn", 4);			/* Wrote too much? */

  (void) strcpy(one, "gh");
  (void) strcpy(two, "ef");
  (void) StrConcat(one, two);
  equal(one, "ghef", 5);			/* Basic test encore. */
  equal(two, "ef", 6);			/* Stomped on source? */

  (void) strcpy(one, "");
  (void) StrConcat(one, "");
  equal(one, "", 7);			/* Boundary conditions. */
  (void) strcpy(one, "ab");
  (void) StrConcat(one, "");
  equal(one, "ab", 8);
  (void) strcpy(one, "");
  (void) StrConcat(one, "cd");
  equal(one, "cd", 9);

  /* strncmp - first test as StrCompare with big counts,
     then test count code.  */
  it = "strncmp";
  check(strncmp("", "", 99) == 0, 1);	/* Trivial case. */
  check(strncmp("a", "a", 99) == 0, 2);	/* Identity. */
  check(strncmp("abc", "abc", 99) == 0, 3);	/* Multicharacter. */
  check(strncmp("abc", "abcd", 99) < 0, 4);	/* Length unequal. */
  check(strncmp("abcd", "abc", 99) > 0, 5);
  check(strncmp("abcd", "abce", 99) < 0, 6);	/* Honestly unequal. */
  check(strncmp("abce", "abcd", 99) > 0, 7);
  check(strncmp("a\203", "a", 2) > 0, 8);	/* Tricky if '\203' < 0 */
  check(strncmp("a\203", "a\003", 2) > 0, 9);
  check(strncmp("abce", "abcd", 3) == 0, 10);	/* Count limited. */
  check(strncmp("abce", "abc", 3) == 0, 11);	/* Count == length. */
  check(strncmp("abcd", "abce", 4) < 0, 12);	/* Nudging limit. */
  check(strncmp("abc", "def", 0) == 0, 13);	/* Zero count. */

  /* strncpy - testing is a bit different because of odd semantics.  */
  it = "strncpy";
  check(strncpy(one, "abc", 4) == one, 1);	/* Returned value. */
  equal(one, "abc", 2);			/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 2);
  equal(one, "xycdefgh", 3);		/* Copy cut by count. */

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 3);		/* Copy cut just before NUL. */
  equal(one, "xyzdefgh", 4);

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 4);		/* Copy just includes NUL. */
  equal(one, "xyz", 5);
  equal(one+4, "efgh", 6);		/* Wrote too much? */

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 5);		/* Copy includes padding. */
  equal(one, "xyz", 7);
  equal(one+4, "", 8);
  equal(one+5, "fgh", 9);

  (void) strcpy(one, "abc");
  (void) strncpy(one, "xyz", 0);		/* Zero-length copy. */
  equal(one, "abc", 10);	

  (void) strncpy(one, "", 2);		/* Zero-length source. */
  equal(one, "", 11);
  equal(one+1, "", 12);	
  equal(one+2, "c", 13);

  (void) strcpy(one, "hi there");
  (void) strncpy(two, one, 9);
  equal(two, "hi there", 14);		/* Just paranoia. */
  equal(one, "hi there", 15);		/* Stomped on source? */

  /* strlen.  */
  it = "strlen";
  check(strlen("") == 0, 1);		/* Empty. */
  check(strlen("a") == 1, 2);		/* Single char. */
  check(strlen("abcd") == 4, 3);		/* Multiple chars. */

  /* StrFindChr.  */
  it = "StrFindChr";
  check(StrFindChr("abcd", 'z') == NULL, 1);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(StrFindChr(one, 'c') == one+2, 2);	/* Basic test. */
  check(StrFindChr(one, 'd') == one+3, 3);	/* End of string. */
  check(StrFindChr(one, 'a') == one, 4);	/* Beginning. */
  check(StrFindChr(one, '\0') == one+4, 5);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(StrFindChr(one, 'b') == one+1, 6);	/* Finding first. */
  (void) strcpy(one, "");
  check(StrFindChr(one, 'b') == NULL, 7);	/* Empty string. */
  check(StrFindChr(one, '\0') == one, 8);	/* NUL in empty string. */

#if 0
  /* index - just like StrFindChr.  */
  it = "index";
  check(index("abcd", 'z') == NULL, 1);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(index(one, 'c') == one+2, 2);	/* Basic test. */
  check(index(one, 'd') == one+3, 3);	/* End of string. */
  check(index(one, 'a') == one, 4);	/* Beginning. */
  check(index(one, '\0') == one+4, 5);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(index(one, 'b') == one+1, 6);	/* Finding first. */
  (void) strcpy(one, "");
  check(index(one, 'b') == NULL, 7);	/* Empty string. */
  check(index(one, '\0') == one, 8);	/* NUL in empty string. */
#endif

  /* strrchr.  */
  it = "strrchr";
  check(strrchr("abcd", 'z') == NULL, 1);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(strrchr(one, 'c') == one+2, 2);	/* Basic test. */
  check(strrchr(one, 'd') == one+3, 3);	/* End of string. */
  check(strrchr(one, 'a') == one, 4);	/* Beginning. */
  check(strrchr(one, '\0') == one+4, 5);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(strrchr(one, 'b') == one+3, 6);	/* Finding last. */
  (void) strcpy(one, "");
  check(strrchr(one, 'b') == NULL, 7);	/* Empty string. */
  check(strrchr(one, '\0') == one, 8);	/* NUL in empty string. */

#if 0
  /* rindex - just like strrchr.  */
  it = "rindex";
  check(rindex("abcd", 'z') == NULL, 1);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(rindex(one, 'c') == one+2, 2);	/* Basic test. */
  check(rindex(one, 'd') == one+3, 3);	/* End of string. */
  check(rindex(one, 'a') == one, 4);	/* Beginning. */
  check(rindex(one, '\0') == one+4, 5);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(rindex(one, 'b') == one+3, 6);	/* Finding last. */
  (void) strcpy(one, "");
  check(rindex(one, 'b') == NULL, 7);	/* Empty string. */
  check(rindex(one, '\0') == one, 8);	/* NUL in empty string. */
#endif

  /* strpbrk - somewhat like StrFindChr.  */
  it = "strpbrk";
  check(strpbrk("abcd", "z") == NULL, 1);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(strpbrk(one, "c") == one+2, 2);	/* Basic test. */
  check(strpbrk(one, "d") == one+3, 3);	/* End of string. */
  check(strpbrk(one, "a") == one, 4);	/* Beginning. */
  check(strpbrk(one, "") == NULL, 5);	/* Empty search list. */
  check(strpbrk(one, "cb") == one+1, 6);	/* Multiple search. */
  (void) strcpy(one, "abcabdea");
  check(strpbrk(one, "b") == one+1, 7);	/* Finding first. */
  check(strpbrk(one, "cb") == one+1, 8);	/* With multiple search. */
  check(strpbrk(one, "db") == one+1, 9);	/* Another variant. */
  (void) strcpy(one, "");
  check(strpbrk(one, "bc") == NULL, 10);	/* Empty string. */
  check(strpbrk(one, "") == NULL, 11);	/* Both strings empty. */

  /* StrFindStr - somewhat like StrFindChr.  */
  it = "StrFindStr";
  check(StrFindStr("abcd", "z") == NULL, 1);	/* Not found. */
  check(StrFindStr("abcd", "abx") == NULL, 2);	/* Dead end. */
  (void) strcpy(one, "abcd");
  check(StrFindStr(one, "c") == one+2, 3);	/* Basic test. */
  check(StrFindStr(one, "bc") == one+1, 4);	/* Multichar. */
  check(StrFindStr(one, "d") == one+3, 5);	/* End of string. */
  check(StrFindStr(one, "cd") == one+2, 6);	/* Tail of string. */
  check(StrFindStr(one, "abc") == one, 7);	/* Beginning. */
  check(StrFindStr(one, "abcd") == one, 8);	/* Exact match. */
  check(StrFindStr(one, "abcde") == NULL, 9);	/* Too long. */
  check(StrFindStr(one, "de") == NULL, 10);	/* Past end. */
  check(StrFindStr(one, "") == one, 11);	/* Finding empty. */
  (void) strcpy(one, "ababa");
  check(StrFindStr(one, "ba") == one+1, 12);	/* Finding first. */
  (void) strcpy(one, "");
  check(StrFindStr(one, "b") == NULL, 13);	/* Empty string. */
  check(StrFindStr(one, "") == one, 14);	/* Empty in empty string. */
  (void) strcpy(one, "bcbca");
  check(StrFindStr(one, "bca") == one+2, 15);	/* False start. */
  (void) strcpy(one, "bbbcabbca");
  check(StrFindStr(one, "bbca") == one+1, 16);	/* With overlap. */

  /* strspn.  */
  it = "strspn";
  check(strspn("abcba", "abc") == 5, 1);	/* Whole string. */
  check(strspn("abcba", "ab") == 2, 2);	/* Partial. */
  check(strspn("abc", "qx") == 0, 3);	/* None. */
  check(strspn("", "ab") == 0, 4);	/* Null string. */
  check(strspn("abc", "") == 0, 5);	/* Null search list. */

  /* strcspn.  */
  it = "strcspn";
  check(strcspn("abcba", "qx") == 5, 1);	/* Whole string. */
  check(strcspn("abcba", "cx") == 2, 2);	/* Partial. */
  check(strcspn("abc", "abc") == 0, 3);	/* None. */
  check(strcspn("", "ab") == 0, 4);	/* Null string. */
  check(strcspn("abc", "") == 3, 5);	/* Null search list. */

  /* strtok - the hard one.  */
  it = "strtok";
  (void) strcpy(one, "first, second, third");
  equal(strtok(one, ", "), "first", 1);	/* Basic test. */
  equal(one, "first", 2);
  equal(strtok((char *)NULL, ", "), "second", 3);
  equal(strtok((char *)NULL, ", "), "third", 4);
  check(strtok((char *)NULL, ", ") == NULL, 5);
  (void) strcpy(one, ", first, ");
  equal(strtok(one, ", "), "first", 6);	/* Extra delims, 1 tok. */
  check(strtok((char *)NULL, ", ") == NULL, 7);
  (void) strcpy(one, "1a, 1b; 2a, 2b");
  equal(strtok(one, ", "), "1a", 8);	/* Changing delim lists. */
  equal(strtok((char *)NULL, "; "), "1b", 9);
  equal(strtok((char *)NULL, ", "), "2a", 10);
  (void) strcpy(two, "x-y");
  equal(strtok(two, "-"), "x", 11);	/* New string before done. */
  equal(strtok((char *)NULL, "-"), "y", 12);
  check(strtok((char *)NULL, "-") == NULL, 13);
  (void) strcpy(one, "a,b, c,, ,d");
  equal(strtok(one, ", "), "a", 14);	/* Different separators. */
  equal(strtok((char *)NULL, ", "), "b", 15);
  equal(strtok((char *)NULL, " ,"), "c", 16);	/* Permute list too. */
  equal(strtok((char *)NULL, " ,"), "d", 17);
  check(strtok((char *)NULL, ", ") == NULL, 18);
  check(strtok((char *)NULL, ", ") == NULL, 19);	/* Persistence. */
  (void) strcpy(one, ", ");
  check(strtok(one, ", ") == NULL, 20);	/* No tokens. */
  (void) strcpy(one, "");
  check(strtok(one, ", ") == NULL, 21);	/* Empty string. */
  (void) strcpy(one, "abc");
  equal(strtok(one, ", "), "abc", 22);	/* No delimiters. */
  check(strtok((char *)NULL, ", ") == NULL, 23);
  (void) strcpy(one, "abc");
  equal(strtok(one, ""), "abc", 24);	/* Empty delimiter list. */
  check(strtok((char *)NULL, "") == NULL, 25);
  (void) strcpy(one, "abcdefgh");
  (void) strcpy(one, "a,b,c");
  equal(strtok(one, ","), "a", 26);	/* Basics again... */
  equal(strtok((char *)NULL, ","), "b", 27);
  equal(strtok((char *)NULL, ","), "c", 28);
  check(strtok((char *)NULL, ",") == NULL, 29);
  equal(one+6, "gh", 30);			/* Stomped past end? */
  equal(one, "a", 31);			/* Stomped old tokens? */
  equal(one+2, "b", 32);
  equal(one+4, "c", 33);

  /* memcmp.  */
  it = "memcmp";
  check(memcmp("a", "a", 1) == 0, 1);	/* Identity. */
  check(memcmp("abc", "abc", 3) == 0, 2);	/* Multicharacter. */
  check(memcmp("abcd", "abce", 4) < 0, 3);	/* Honestly unequal. */
  check(memcmp("abce", "abcd", 4) > 0, 4);
  check(memcmp("alph", "beta", 4) < 0, 5);
  check(memcmp("a\203", "a\003", 2) > 0, 6);
  check(memcmp("abce", "abcd", 3) == 0, 7);	/* Count limited. */
  check(memcmp("abc", "def", 0) == 0, 8);	/* Zero count. */
  /* strcat.  */
  it = "strcat";
  (void) strcpy(one, "ijk");
  check(strcat(one, "lmn") == one, 1);	/* Returned value. */
  equal(one, "ijklmn", 2);		/* Basic test. */

  (void) strcpy(one, "x");
  (void) strcat(one, "yz");
  equal(one, "xyz", 3);			/* Writeover. */
  equal(one+4, "mn", 4);			/* Wrote too much? */

  (void) strcpy(one, "gh");
  (void) strcpy(two, "ef");
  (void) strcat(one, two);
  equal(one, "ghef", 5);			/* Basic test encore. */
  equal(two, "ef", 6);			/* Stomped on source? */

  (void) strcpy(one, "");
  (void) strcat(one, "");
  equal(one, "", 7);			/* Boundary conditions. */
  (void) strcpy(one, "ab");
  (void) strcat(one, "");
  equal(one, "ab", 8);
  (void) strcpy(one, "");
  (void) strcat(one, "cd");
  equal(one, "cd", 9);
  
  /* memchr.  */
  it = "memchr";
  check(memchr("abcd", 'z', 4) == NULL, 1);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(memchr(one, 'c', 4) == one+2, 2);	/* Basic test. */
  check(memchr(one, 'd', 4) == one+3, 3);	/* End of string. */
  check(memchr(one, 'a', 4) == one, 4);	/* Beginning. */
  check(memchr(one, '\0', 5) == one+4, 5);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(memchr(one, 'b', 5) == one+1, 6);	/* Finding first. */
  check(memchr(one, 'b', 0) == NULL, 7);	/* Zero count. */
  check(memchr(one, 'a', 1) == one, 8);	/* Singleton case. */
  (void) strcpy(one, "a\203b");
  check(memchr(one, 0203, 3) == one+1, 9);	/* Unsignedness. */

  /* memcpy - need not work for overlap.  */
  it = "memcpy";
  check(memcpy(one, "abc", 4) == one, 1);	/* Returned value. */
  equal(one, "abc", 2);			/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) memcpy(one+1, "xyz", 2);
  equal(one, "axydefgh", 3);		/* Basic test. */

  (void) strcpy(one, "abc");
  (void) memcpy(one, "xyz", 0);
  equal(one, "abc", 4);			/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) memcpy(two, one, 9);
  equal(two, "hi there", 5);		/* Just paranoia. */
  equal(one, "hi there", 6);		/* Stomped on source? */

  /* memmove - must work on overlap.  */
  it = "memmove";
  check(memmove(one, "abc", 4) == one, 1);	/* Returned value. */
  equal(one, "abc", 2);			/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one+1, "xyz", 2);
  equal(one, "axydefgh", 3);		/* Basic test. */

  (void) strcpy(one, "abc");
  (void) memmove(one, "xyz", 0);
  equal(one, "abc", 4);			/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) memmove(two, one, 9);
  equal(two, "hi there", 5);		/* Just paranoia. */
  equal(one, "hi there", 6);		/* Stomped on source? */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one+1, one, 9);
  equal(one, "aabcdefgh", 7);		/* Overlap, right-to-left. */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one+1, one+2, 7);
  equal(one, "acdefgh", 8);		/* Overlap, left-to-right. */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one, one, 9);
  equal(one, "abcdefgh", 9);		/* 100% overlap. */

  /* memccpy - first test like memcpy, then the search part
     The SVID, the only place where memccpy is mentioned, says
     overlap might fail, so we don't try it.  Besides, it's hard
     to see the rationale for a non-left-to-right memccpy.  */
  
  /* memset.  */
  it = "memset";
  (void) strcpy(one, "abcdefgh");
  check(memset(one+1, 'x', 3) == one+1, 1);	/* Return value. */
  equal(one, "axxxefgh", 2);		/* Basic test. */

  (void) memset(one+2, 'y', 0);
  equal(one, "axxxefgh", 3);		/* Zero-length set. */

  (void) memset(one+5, 0, 1);
  equal(one, "axxxe", 4);			/* Zero fill. */
  equal(one+6, "gh", 5);			/* And the leftover. */

  (void) memset(one+2, 010045, 1);
  equal(one, "ax\045xe", 6);		/* Unsigned char convert. */

  /* bcopy - much like memcpy.
     Berklix manual is silent about overlap, so don't test it.  */
  it = "bcopy";
  (void) bcopy("abc", one, 4);
  equal(one, "abc", 1);			/* Simple copy. */

  (void) strcpy(one, "abcdefgh");
  (void) bcopy("xyz", one+1, 2);
  equal(one, "axydefgh", 2);		/* Basic test. */

  (void) strcpy(one, "abc");
  (void) bcopy("xyz", one, 0);
  equal(one, "abc", 3);			/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) bcopy(one, two, 9);
  equal(two, "hi there", 4);		/* Just paranoia. */
  equal(one, "hi there", 5);		/* Stomped on source? */

  /* bzero.  */
  it = "bzero";
  (void) strcpy(one, "abcdef");
  bzero(one+2, 2);
  equal(one, "ab", 1);			/* Basic test. */
  equal(one+3, "", 2);
  equal(one+4, "ef", 3);

  (void) strcpy(one, "abcdef");
  bzero(one+2, 0);
  equal(one, "abcdef", 4);		/* Zero-length copy. */

#if 0
  /* bcmp - somewhat like memcmp.  */
  it = "bcmp";
  check(bcmp("a", "a", 1) == 0, 1);	/* Identity. */
  check(bcmp("abc", "abc", 3) == 0, 2);	/* Multicharacter. */
  check(bcmp("abcd", "abce", 4) != 0, 3);	/* Honestly unequal. */
  check(bcmp("abce", "abcd", 4) != 0, 4);
  check(bcmp("alph", "beta", 4) != 0, 5);
  check(bcmp("abce", "abcd", 3) == 0, 6);	/* Count limited. */
  check(bcmp("abc", "def", 0) == 0, 8);	/* Zero count. */
#endif

  {
    char text[] = "This,is,a,test";
    char *list = text;
    it = "strsep";
    check (!StrCompare ("This", strsep (&list, ",")), 1);
    check (!StrCompare ("is", strsep (&list, ",")), 2);
    check (!StrCompare ("a", strsep (&list, ",")), 3);
    check (!StrCompare ("test", strsep (&list, ",")), 4);
    check (strsep (&list, ",") == NULL, 5);
  }

  /* strerror - VERY system-dependent.  */
  {
    int f;
    it = "strerror";
    // f = __open("/", O_WRONLY);	/* Should always fail. */
    // check(f < 0 && errno > 0 && errno < _sys_nerr, 1);
    // equal(strerror(errno), _sys_errlist[errno], 2);
  }

  {
    int status;
    if (errors == 0)
      {
	status = EXIT_SUCCESS;
	puts("No errors.");
      }
    else
      {
	status = EXIT_FAILURE;
	printf("%ld errors.\n", errors);
      }
    exit(status);
  }
}
