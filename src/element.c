#include <chtml/element.h>

void chtml_element_init(chtml_element** el) {
	*el = (chtml_element*) malloc(sizeof(chtml_element));
	(*el)->next = NULL;
	(*el)->child = NULL;
	(*el)->attributes = NULL;
	(*el)->content = NULL;
	(*el)->attributes_size = 0;
}

void chtml_element_add_content(chtml_element** el, string str) {
	size_t str_s = strlen(str), curr_str_s;
	if((*el)->content == NULL) curr_str_s = 0;
	else curr_str_s = strlen((*el)->content);
	(*el)->content = (string) realloc((*el)->content, curr_str_s + str_s + 1);
	strcat((*el)->content, str);
}

void chtml_element_add_attribute(chtml_element** el, chtml_attribute* attr) {

}

void chtml_element_delete(chtml_element** el) {
	if(*el == NULL) return;
	chtml_element_delete(&(*el)->child);
	chtml_element_delete(&(*el)->next);
	for(int i = 0; i < (*el)->attributes_size; i++) chtml_attribute_delete(&(*el)->attributes[i]);
	if((*el)->content != NULL)
		free((*el)->content);
	free(*el);
}
