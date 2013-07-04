/* 
 * File:   english_stemmer.h
 * Author: phrk
 *
 * Created on July 4, 2013, 6:27 PM
 */

#ifndef ENGLISH_STEMMER_H
#define	ENGLISH_STEMMER_H

#include "porter_stemmer.h"
#include <string>

class HenglishStemmer : public HwordStemmer
{
    stemmer *z;
public:
    
    HenglishStemmer()
    {
        z = create_stemmer();
    }
    
    ~HenglishStemmer()
    {
        free_stemmer(z);
    }
    
    virtual string stemWord(string word)
    {
        if (word.size()>=50)
        {
            word = word.substr(0, 49);
        }
        char s[50];
        strcpy(s, word.c_str());
        
        if (word.size()>1)
        {
            if ('A' <= s[1] && s[1] <= 'Z')
            {

            }
            else
            {
                    s[0] = tolower(s[0]);
            }
        }
        else
        {
                s[0] = tolower(s[0]);
        }
        
        s[stem(z, s, word.size()-1) + 1] = 0;
        return string(s);
    }
};

#endif	/* ENGLISH_STEMMER_H */

