# chtml
Small parser for html/xhtml in C.

### Include a header
```c
#include <chtml.h>
```

### Initializing root element
```c
chtml_element* root;
int return_value = chtml_parser_parse_file("test.html", &root);
if(return_value) {
	//... error while parsing
} else {
	// DOM is created
}
chtml_element_delete(&root); // removes the given element
```

### Element properties
```c
// example code for outputing root element name, content and first attribute key and value
#include <chtml.h>

int main(int argc, char** argv) 
{
	chtml_element* root;
	if(argc > 1) {
		if(!chtml_parser_parse_file(argv[1], &root)) {
			printf("Root element: \n");
			printf("\t name -> %s\n", root->tag);
			printf("\t content -> %s\n", root->content);
			printf("\t first attribute -> %s::'%s'\n", root->attributes[0]->key, root->attributes[0]->value);
		}
	}
	chtml_element_delete(&root);
	return 0;
}
```


 
