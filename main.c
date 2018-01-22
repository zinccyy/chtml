#include <stdio.h>
#include <chtml.h>

int main(int argc, char** argv) 
{
	chtml_element* root;
	if(argc > 1) {
		chtml_parser_parse_file(argv[1], &root);
	}
	return 0;
}
