#include <chtml/element_stack.h>

void chtml_element_stack_init(chtml_element_stack* stack) {
	stack->elements = NULL;
	stack->elements_size = 0;
}

void chtml_element_stack_push(chtml_element_stack* stack, chtml_element* el) {
	stack->elements = (chtml_element**) realloc(stack->elements, sizeof(chtml_element*) * (stack->elements_size + 1));
	*(stack->elements + stack->elements_size) = el;
	stack->elements_size += 1;
}

chtml_element* chtml_element_stack_pop(chtml_element_stack* stack) {
	chtml_element* el = NULL;
	if(stack->elements_size) {
		el = stack->elements[stack->elements_size-1];
		stack->elements = (chtml_element**) realloc(stack->elements, sizeof(chtml_element*) * (stack->elements_size - 1));
		stack->elements_size -= 1;
	}
	return el;
}

void chtml_element_stack_delete(chtml_element_stack* stack) {
	if(stack->elements) {
		free(stack->elements);
		stack->elements_size = 0;
	}
}
