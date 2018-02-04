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
	temp[i] = '\0';
}

int chtml_parser_add_element(chtml_element** current_element, chtml_element_stack* el_stack, string name) {
	chtml_element* el, *itr_element;
	chtml_element_init(&el);
	chtml_element_set_tag(&el, name);
	if(el_stack->elements_size) { // check if the last element on the stack has children and if so, add the new element as the next of the last child
		chtml_element* last = chtml_element_stack_last(el_stack);
		if(last->child) {
			for(itr_element = last->child; itr_element->next; itr_element = itr_element->next);
				itr_element->next = el;
		} else last->child = el;
						
	}
	chtml_element_stack_push(el_stack, el);
	*current_element = el;
	//printf("NAME: %s\n", (*current_element)->tag);
	return 0;
}

int chtml_parser_add_empty_element(chtml_element** current_element, chtml_element_stack* el_stack, string name) {
	return 0;
}

int chtml_parser_parse_tag(chtml_element** current_element, chtml_element_stack* el_stack, const char* str, int start, int end, int* line_number) {
	int i, last, j, ret_value = 0;
	string temp_word, temp_string = NULL;
	chtml_attribute* temp_attr;
	// TODO -> add attributes and improve error checking
	
	if(str[start+1] == '!' || str[start+1] == '?') return 0;

	enum {
		GetNameTagParsingSignal,
		GetAttributeNameParsingSignal,
		GetAttributeValueParsingSignal,
		GetStringEndParsingSignal,
		GetEndTagNameParsingSignal
	} TagParsingSignal = GetNameTagParsingSignal;
	
	int endTag = 0;
	for(i = start+1; i <= end; i++) {
		if(str[i] != ' ') {
			if(str[i] == '/') endTag = 1;
			break;
		}
	}
	if(str[i] == '/') {
		++i;
		TagParsingSignal = GetEndTagNameParsingSignal;
	}
	
	for(last = i; i <= end; i++) {
		if(str[i] == '\"') {
			if(TagParsingSignal == GetStringEndParsingSignal) {
				// close the string and add an attribute
				temp_string = (string) malloc(sizeof(char) * (i-last));
				chtml_parser_create_substring(str, temp_string, last, i);
				// create an attribute and add it to the current element
				chtml_attribute_init(&temp_attr);
				chtml_attribute_set_key(&temp_attr, temp_word);
				chtml_attribute_set_value(&temp_attr, temp_string);
				chtml_element_add_attribute(current_element, &temp_attr);
				free(temp_string);
				free(temp_word);
				TagParsingSignal = GetAttributeNameParsingSignal;
				last = i+1;
				continue;
			} else if(TagParsingSignal == GetAttributeValueParsingSignal) {
				// init temp_string and add characters to it until the end is reached
				last = i+1;
				TagParsingSignal = GetStringEndParsingSignal;
			} else {
				// error -> random string
			}
		} else if(str[i] == ' ') {
			if(TagParsingSignal == GetNameTagParsingSignal || TagParsingSignal == GetEndTagNameParsingSignal) {
				temp_word = (string) malloc(sizeof(char) * (i-last));
				chtml_parser_create_substring(str, temp_word, last, i);
				if(TagParsingSignal == GetEndTagNameParsingSignal) {
					// remove the last element from the stack
					chtml_element_stack_pop(el_stack);
				} else {
					// create an element and add it to the stack
					chtml_parser_add_element(current_element, el_stack, temp_word);
				}
				free(temp_word);
				if(TagParsingSignal == GetEndTagNameParsingSignal) {
					break;
				}
				TagParsingSignal = GetAttributeNameParsingSignal;
				++i;
				last = i;
			} else if(TagParsingSignal == GetAttributeNameParsingSignal) {
				// do nothing
				if(i-last) {
					// error -> example: <html attr = ... <-- attr __space__ 
				} else {
					last = i+1;
					continue;
				}
			}
		} else if(str[i] == '/') {
			// empty element -> create an element without content
			if(TagParsingSignal == GetNameTagParsingSignal) {
				temp_word = (string) malloc(sizeof(char) * (i-last));
				chtml_parser_create_substring(str, temp_word, last, i);
				chtml_parser_add_element(current_element, el_stack, temp_word);
				chtml_element_stack_pop(el_stack); // pop the created element from the stack and end
				free(temp_word);
				break;
			} else if(TagParsingSignal == GetAttributeNameParsingSignal) {
				// after adding all attributes, pop the empty element from the stack and end the loop
				if(i-last) {
					fprintf(stderr, "Error at line %d -> random '/' character.\n", *line_number);
					ret_value = 1;
					break;
				}
				chtml_element_stack_pop(el_stack);
				break;
			} else if(TagParsingSignal == GetAttributeValueParsingSignal) {
				// error -> random '/'
				fprintf(stderr, "Error at line %d -> random '/' character.\n", *line_number);
				ret_value = 1;
				break;
			}
		} else if(str[i] == '=') {
			if(TagParsingSignal == GetAttributeNameParsingSignal) {
				temp_word = (string)malloc(sizeof(char) * (i-last) );
				chtml_parser_create_substring(str, temp_word, last, i);
				TagParsingSignal = GetAttributeValueParsingSignal;
			} else if(TagParsingSignal == GetAttributeValueParsingSignal) {
			} 
		} else if(str[i] == '>') {
			if(TagParsingSignal == GetNameTagParsingSignal || TagParsingSignal == GetEndTagNameParsingSignal) {
				temp_word = (string) malloc(sizeof(char) * (i-last));
				chtml_parser_create_substring(str, temp_word, last, i);
				if(TagParsingSignal == GetEndTagNameParsingSignal) {
					chtml_element_stack_pop(el_stack);
				} else {
					chtml_parser_add_element(current_element, el_stack, temp_word);
				}
				free(temp_word);
			} else {
			}
		} else {
			
		}
	}
	
	/*for(i = 0; i < *indent; i++) printf("    ");
	printf("[ ");
	for(i = start+1; i <= end; i++) printf("%c", str[i]);
	printf(" ]\n");
	
	if(str[start+1] != '/') {
		// push the element on the stack
		*(indent) += 1;
	}*/
	return ret_value;
}

int chtml_parser_parse_content(chtml_element** current_element, chtml_element_stack* el_stack, const char* str, int start, int end) {
	int i;
	string content = (string) malloc(end-start+1);
	chtml_parser_create_substring(str, content, start, end+1);
	//printf("CONTENT [%s]: %s\n", (*current_element)->tag, content);
	chtml_element_add_content(current_element, content);
	free(content);
	// add the content to the current element being proccesed
	/*for(i = 0; i < *indent; i++) printf("    ");
	printf("CONTENT: [%d] -> ", end-start);
	for(i = start; i <= end; i++) {
		putc(str[i], stdout);
	}
	if(str[i-1] != '\n')
		printf("\n");*/
	return 0;
}

int chtml_parser_parse_string(const char* str, chtml_element** root_element) { 
	*root_element = NULL;
	int i, return_value = 0, line_number = 1, start, end, indent = 0;
	chtml_element_stack el_stack;
	chtml_element* root_bkp = *root_element;
	chtml_element** current_element = root_element;
	
	chtml_element_stack_init(&el_stack);
	CurrentParsingSignal = NormalParsingSignal;
	
	for(i = 0; str[i]; i++) {
		if(str[i] == '<') {
			if(CurrentParsingSignal == ContentParsingSignal) {
				end = i-1;
				if(end-start > 0) {
					chtml_parser_parse_content(current_element, &el_stack, str, start, end);
					// parse content and set signal to add a child element
				}
			}
			CurrentParsingSignal = ElementTagParsingSignal;
			start = i;
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
			end = i;
			if(chtml_parser_parse_tag(current_element, &el_stack, str, start, end, &line_number)) {
				return_value = 1;
				break;
			}
			start = i+1;
			if(!root_bkp && *current_element) {
				root_bkp = *current_element;
			}
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
	*root_element = root_bkp;
	chtml_element_stack_delete(&el_stack);
	return return_value;
}
