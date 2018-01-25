#include <chtml/parser.h>

int chtml_parser_parse_file(const char* fn, chtml_element** root_element) {
	FILE* file = fopen(fn, "r");
	size_t file_size;
	string content;
	int return_value;
	if(file == NULL) {
		fprintf(stderr, "%s\n", strerror(errno));
		return errno;
	} else {
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
		content = (string) malloc(file_size+1);
		fread(content, sizeof(char), file_size, file);
		return_value = chtml_parser_parse_string(content, root_element);
		free(content);
		fclose(file);
	}
	return return_value;
}

// parses html/xml/xhtml content

int is_token(char c) {
	return(c == '\"' || c == ' ' || c == '/' || c == '=');
}

void chtml_parser_create_substring(const char* str, string temp, int s, int e) {
	int i;
	for(i = 0; i < e-s; i++)
		temp[i] = str[i+s];
}

int chtml_parser_parse_tag(chtml_element** current_element, chtml_element_stack* el_stack,const char* str, int start, int end, int* indent) { // indent is used for tests
	int i, last, j;
	string temp_word, temp_string;
	
	if(str[start] == '!' || str[start] == '?') return 0;

	enum {
		GetNameTagParsingSignal,
		GetAttributeNameParsingSignal,
		GetAttributeValueParsingSignal,
		GetEndTagNameParsingSignal
	} TagParsingSignal = GetNameTagParsingSignal;
	
	if(str[start] == '/') {
		*(indent) -= 1;
		TagParsingSignal = GetEndTagNameParsingSignal;
		start++;
		// get the name and remove the last element from the stack
	} else TagParsingSignal = GetNameTagParsingSignal;
	
	for(i = start, last = i; i <= end; i++) {
		if(is_token(str[i])) {
			switch(str[i]) {
				case ' ':
					if(i-last) {
						if(TagParsingSignal == GetNameTagParsingSignal || TagParsingSignal == GetEndTagNameParsingSignal) {
							// get tag
							temp_word = (string) malloc(sizeof(char) * (i-last+1));
							chtml_parser_create_substring(str, temp_word, last, i);
							if(TagParsingSignal == GetEndTagNameParsingSignal) {
								// pop back from stack to get the last element and check if the tag fits -> if not -> error
								
								break;
							} else {
								
							}
							free(temp_word);
							TagParsingSignal = GetAttributeNameParsingSignal;
						}
					}
					break;
				case '/':
					break;
				case '=':
					if(TagParsingSignal == GetAttributeNameParsingSignal) {
						
					} else {
						// error
					}
					break;
				case '\"':
					break;
			}
			last = i+1;
		}
	}
	
	if(i-last) {
		temp_word = (string) malloc(sizeof(char) * (i-last+1));
		chtml_parser_create_substring(str, temp_word, last, i);
		free(temp_word);
	}
	
	if(str[start] != '/') {
		// push the element on the stack
		*(indent) += 1;
	}
	return 0;
}

int chtml_parser_parse_content(chtml_element** current_element, const char* str, int start, int end, int* indent) {
	int i;
	// add the content to the current element being proccesed
	for(i = 0; i < *indent; i++) printf("    ");
	printf("CONTENT: [%d] -> ", end-start);
	for(i = start; i <= end; i++) {
		putc(str[i], stdout);
	}
	if(str[i-1] != '\n')
		printf("\n");
	return 0;
}

int chtml_parser_parse_string(const char* str, chtml_element** root_element) { 
	int i, return_value = 0, line_number = 1, start, end, indent = 0;
	chtml_element_stack el_stack;
	chtml_element* root_bkp = *root_element;
	chtml_element** current_element = root_element;
	
	chtml_element_init(root_element);
	chtml_element_stack_init(&el_stack);
	
	CurrentParsingSignal = NormalParsingSignal;
	
	for(i = 0; str[i]; i++) {
		if(str[i] == '<') {
			if(CurrentParsingSignal == ContentParsingSignal) {
				end = i-1;
				if(end-start > 0) {
					//chtml_parser_parse_content(current_element, str, start, end, &indent);
					// parse content
				}
			}
			CurrentParsingSignal = ElementTagParsingSignal;
			start = i+1;
			continue;
		} else if(str[i] == '>') {
			if(CurrentParsingSignal == DoctypeParsingSignal) {
				CurrentParsingSignal = ContentParsingSignal;
				//continue;
			} else if(CurrentParsingSignal == ElementTagParsingSignal) {
				// parse element tag and add an element to elements
				CurrentParsingSignal = ContentParsingSignal;
				//continue;
			} else if(CurrentParsingSignal == XmlEncodingParsingSignal) {
				fprintf(stderr, "Tag closed and xml encoding not finished at line %d.\n", line_number);
				return_value = 1;
				break;
			}
			end = i-1;
			chtml_parser_parse_tag(current_element, &el_stack, str, start, end, &indent);
			start = i+1;
			continue;
		} else if(str[i] == '!') {
			if(CurrentParsingSignal == ElementTagParsingSignal) {
				CurrentParsingSignal = DoctypeParsingSignal;
			} else if(CurrentParsingSignal == NormalParsingSignal) {
				fprintf(stderr, "Invalid character '!' at line %d\n", line_number);
				return_value = 1;
				break;
			}
		} else if(str[i] == '?') {
			if(CurrentParsingSignal == ElementTagParsingSignal) {
				CurrentParsingSignal = XmlEncodingParsingSignal;
			} else if(CurrentParsingSignal == XmlEncodingParsingSignal) {
				CurrentParsingSignal = ElementTagParsingSignal;
			} else {
				fprintf(stderr, "Invalid character '?' at line %d\n", line_number);
				return_value = 1;
				break;
			}
		} else if(str[i] == '\n') {
			line_number++;
		}
	}
	chtml_element_stack_delete(&el_stack);
	return return_value;
}
