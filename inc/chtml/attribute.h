#ifndef __CHTML_ATTRIBUTE_H__
#define __CHTML_ATTRIBUTE_H__

#include <chtml/types.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	string key;
	string value;
} chtml_attribute;

void chtml_attribute_init(chtml_attribute* );
void chtml_attribute_set_key(chtml_attribute*, const char* );
void chtml_attribute_set_value(chtml_attribute*, const char* );
void chtml_attribute_delete(chtml_attribute* );

#endif
