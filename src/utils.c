#include <utils.h>

size_t search(const char * arr, const size_t size, const char c)
{
   size_t i=0;
   while(i<size)
      if (c == arr[i++])
	 return i;
   return 0;
}

/* Just the help function */
void help()
{
   /* Pretty print */
   printf("ezPass - Password Manager\n");
   printf("\t-l, --length      : to specify the length of the password.\n"   );
   printf("\t-n, --not-admitted: to specify which type of character is not\n");
   printf("\t                    allowed [digit - u_char - l_char - sign]\n" );
   printf("\t-t, --times       : to specify how many password you need\n"    );
   printf("\t    --print       : to print the stats\n"  );
   printf("\t    --help        : to show this message\n");
   printf("\t    --version     : to print the current version of the program\n");
}

size_t count_digits(float num) {
	float second_part = num - (int)num;
	size_t counter = 0, dec_digits=3;

  if (!num)
    return 1;
	//counting first part of digits
	counter = log10((int)num)+1;
  if (!second_part)
    return counter;

	while(dec_digits--)
		second_part*=10;

	//approximation
	if (((second_part - (int)second_part)*10) > 4)
		second_part+=1;
	counter += log10((int)second_part);
	
	return counter;
}
