#include <chtml.h>

void printTree(chtml_element* root, int a) {
	int i;
	chtml_element* el = root;
	if(!root) return;
	for(; el; el = el->next) {
		for(i = 0; i < a; i++) printf("    ");
		printf("%s [ ", el->tag);
		if(el->attributes_size) {
			for(i = 0; i < el->attributes_size; i++) {
				printf("%s::'%s' ", el->attributes[i]->key, el->attributes[i]->value);
			}
		}
		printf("]\n");
		for(i = 0; i < a; i++) printf("    ");
		printf("CONTENT : %s\n", el->content);
		printTree(el->child, a+1);
	}
}

int main(int argc, char** argv) 
{
	chtml_element* root;
	if(argc > 1) {
		chtml_parser_parse_file(argv[1], &root);
		//printTree(root, 0);
	}
	chtml_element_delete(&root);
	return 0;
}
