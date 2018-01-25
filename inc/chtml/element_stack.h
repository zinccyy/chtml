#ifndef __CHTML_ELEMENT_STACK_H__
#define __CHTML_ELEMENT_STACK_H__

#include <chtml/element.h>

typedef struct {
	chtml_element** elements;
	size_t elements_size;
} chtml_element_stack;

void chtml_element_stack_init(chtml_element_stack* );
void chtml_element_stack_push(chtml_element_stack*, chtml_element* );
chtml_element* chtml_element_stack_pop(chtml_element_stack* );
void chtml_element_stack_delete(chtml_element_stack* );

#endif
