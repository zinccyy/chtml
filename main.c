#include <chtml.h>

int main(int argc, char** argv) 
{
	/*chtml_attribute* attr;
	chtml_attribute_init(&attr);
	chtml_attribute_set_key(&attr, "OK");
	chtml_attribute_set_value(&attr, "OK2");
	
	chtml_element* element;
	chtml_element_init(&element);
	chtml_element_set_tag(&element, "TAG");
	chtml_element_add_content(&element, "HEHEHEH");
	chtml_element_add_content(&element, " OK");
	chtml_element_add_attribute(&element, &attr);
	printf("%s\n", element->content);
	
	for(int i= 0; i < element->attributes_size; i++) {
		printf("%s='%s'\n", element->attributes[i]->key, element->attributes[i]->value);
	}
	
	chtml_element_stack stack;
	chtml_element_stack_init(&stack);
	chtml_element_stack_push(&stack, element);
	chtml_element* el = chtml_element_stack_pop(&stack);
	chtml_element_add_content(&el, " ANOTHER ONE");
	
	printf("%s - - -\n", element->content);
	chtml_element_stack_delete(&stack);
	
	chtml_element_delete(&element);*/
	
	chtml_element* root;
	if(argc > 1) {
		chtml_parser_parse_file(argv[1], &root);
	}
	chtml_element_delete(&root);
	return 0;
}
