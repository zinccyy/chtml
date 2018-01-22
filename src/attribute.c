#include <chtml/attribute.h>

void chtml_attribute_init(chtml_attribute* attr) {
	attr->key = NULL;
	attr->value = NULL;
}

void chtml_attribute_set_key(chtml_attribute* attr, const char* str) {
	if(attr->key) return; // can't change a key or value
	size_t len = strlen(str);
	attr->key = (string) malloc(len+1);
	strcpy(attr->key, str);
}

void chtml_attribute_set_value(chtml_attribute* attr, const char* str) {
	if(attr->value) return; // can't change a key or value
	size_t len = strlen(str);
	attr->value = (string) malloc(len+1);
	strcpy(attr->value, str);
}

void chtml_attribute_delete(chtml_attribute* attr) {
	if(attr->key) free(attr->key);
	if(attr->value) free(attr->value);
}	
