#ifndef _MKFRT_DICTIONARY_H_
#define _MKFRT_DICTIONARY_H_

typedef struct dictionary * Dictionary;

Dictionary makeDictionary();

char *getValueDictionary(Dictionary dict, char *key);

int insertDictionary(Dictionary dict, char *key, char *value);

int removeDictionary(Dictionary dict, char *key);

void foreachDictionary(Dictionary dict, GHFunc func);

int sizeDictionary(Dictionary dict);

void destroyDictionary(Dictionary dict);

#endif