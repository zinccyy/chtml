#ifndef __CHTML_ELEMENT_H__
#define __CHTML_ELEMENT_H__

#include <chtml/attribute.h>
#include <string.h>

typedef struct chtml_element {
	struct chtml_element* next;
	struct chtml_element* child;
	chtml_attribute** attributes;
	string tag;
	string content;
	size_t attributes_size;
} chtml_element;

void chtml_element_init(chtml_element**);
void chtml_element_set_tag(chtml_element**, string);
void chtml_element_add_content(chtml_element**, string);
void chtml_element_add_attribute(chtml_element**, chtml_attribute**);
void chtml_element_delete(chtml_element**);

#endif
