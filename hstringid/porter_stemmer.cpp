#include "porter_stemmer.h"

stemmer * create_stemmer(void)
{
    return (struct stemmer *) malloc(sizeof(struct stemmer));
    /* assume malloc succeeds */
}

void free_stemmer(struct stemmer * z)
{
    free(z);
}



int stem(struct stemmer * z, char * b, int k)
{
   if (k <= 1) return k; /*-DEPARTURE-*/
   z->b = b; z->k = k; /* copy the parameters into z */

   /* With this line, strings of length 1 or 2 don't go through the
      stemming process, although no mention is made of this in the
      published algorithm. Remove the line to match the published
      algorithm. */

   step1ab(z); step1c(z); step2(z); step3(z); step4(z); step5(z);
   return z->k;
}