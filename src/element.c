#include <chtml/element.h>
#include <stdio.h>

void chtml_element_init(chtml_element** el) {
	*el = (chtml_element*) malloc(sizeof(chtml_element));
	(*el)->next = NULL;
	(*el)->child = NULL;
	(*el)->attributes = NULL;
	(*el)->content = NULL;
	(*el)->tag = NULL;
	(*el)->attributes_size = 0;
}

void chtml_element_set_tag(chtml_element** el, string t) {
	size_t tag_size = strlen(t);
	(*el)->tag = (string) malloc(sizeof(char) * (tag_size+1));
	strcpy((*el)->tag, t);
}

void chtml_element_add_content(chtml_element** el, string str, size_t str_s) {
	size_t curr_str_s;
	if((*el)->content == NULL) curr_str_s = 0;
	else curr_str_s = strlen((*el)->content);
	(*el)->content = (string) realloc((*el)->content, sizeof(char) * (curr_str_s + str_s + 1));
	if(!curr_str_s) strcpy((*el)->content, str);
	else strcat((*el)->content, str);
}

void chtml_element_add_attribute(chtml_element** el, chtml_attribute** attr) {
	(*el)->attributes = (chtml_attribute**) realloc((*el)->attributes, sizeof(chtml_attribute*) * ((*el)->attributes_size+1));
	*((*el)->attributes + (*el)->attributes_size) = *attr;
	(*el)->attributes_size+=1;
}

void chtml_element_delete(chtml_element** el) {
	if(*el == NULL) return;
	chtml_element_delete(&(*el)->child);
	chtml_element_delete(&(*el)->next);
	if((*el)->attributes) {
		for(int i = 0; i < (*el)->attributes_size; i++) 
			chtml_attribute_delete(&(*el)->attributes[i]);
		free((*el)->attributes);
	}
	if((*el)->content != NULL)
		free((*el)->content);
	if((*el)->tag != NULL)  {
		free((*el)->tag);
	}
	free(*el);
}
