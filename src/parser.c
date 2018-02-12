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

int chtml_parser_add_element(chtml_element** current_element, chtml_element** last_element, chtml_element_stack* el_stack, string name) {
	chtml_element* el;
	chtml_element_init(&el);
	chtml_element_set_tag(&el, name);

	if(el_stack->elements_size) { // check if the last element on the stack has children and if so, add the new element as the next of the last child
		chtml_element* last = chtml_element_stack_last(el_stack);
		if(last->child) {
			(*last_element)->next = el;
		} else last->child = el;
						
	}
	chtml_element_stack_push(el_stack, el);
	*current_element = el;
	return 0;
}

int chtml_parser_parse_tag(chtml_element** current_element, chtml_element** last_element, chtml_element_stack* el_stack, const char* str, int start, int* end, int* line_number) {
	int i, last, ret_value = 0;
	string temp_word = NULL, temp_string = NULL;
	chtml_attribute* temp_attr;

	if(str[start] == '!' || str[start] == '?') {
		for(i = start+1; str[i] && str[i] != '>'; ++i); // if end of a tag is not reached, loop till the end
		*end = i;
		return 0;
	}

	enum {
		GetNameTagParsingSignal,
		GetAttributeNameParsingSignal,
		GetAttributeValueParsingSignal,
		GetStringEndParsingSignal,
		GetEndTagNameParsingSignal
	} TagParsingSignal = GetNameTagParsingSignal;
	
	for(i = start; str[i]; i++)
		if(str[i] != ' ') 
			break;
	if(str[i] == '/') {
		++i;
		TagParsingSignal = GetEndTagNameParsingSignal;
	}
	
	for(last = i; str[i]; i++) {
		if(str[i] == '\"') {
			if(TagParsingSignal == GetStringEndParsingSignal) {
				// close the string and add an attribute
				temp_string = (string) malloc(sizeof(char) * (i-last+1));
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
				temp_word = (string) malloc(sizeof(char) * (i-last+1));
				chtml_parser_create_substring(str, temp_word, last, i);
				if(TagParsingSignal == GetEndTagNameParsingSignal) {
					// remove the last element from the stack
					*last_element = chtml_element_stack_pop(el_stack);
					*current_element = chtml_element_stack_last(el_stack);
				} else {
					// create an element and add it to the stack
					chtml_parser_add_element(current_element, last_element, el_stack, temp_word);
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
				temp_word = (string) malloc(sizeof(char) * (i-last+1));
				chtml_parser_create_substring(str, temp_word, last, i);
				chtml_parser_add_element(current_element, last_element, el_stack, temp_word);
				*last_element = chtml_element_stack_pop(el_stack); // pop the created element from the stack and end
				free(temp_word);
				break;
			} else if(TagParsingSignal == GetAttributeNameParsingSignal) {
				// after adding all attributes, pop the empty element from the stack and end the loop
				if(i-last) {
					fprintf(stderr, "Error at line %d -> random '/' character.\n", *line_number);
					ret_value = 1;
					break;
				}
				*last_element = chtml_element_stack_pop(el_stack);
				*current_element = chtml_element_stack_last(el_stack);
				break;
			} else if(TagParsingSignal == GetAttributeValueParsingSignal) {
				// error -> random '/'
				fprintf(stderr, "Error at line %d -> random '/' character.\n", *line_number);
				ret_value = 1;
				break;
			}
		} else if(str[i] == '=') {
			if(TagParsingSignal == GetAttributeNameParsingSignal) {
				temp_word = (string)malloc(sizeof(char) * (i-last+1) );
				chtml_parser_create_substring(str, temp_word, last, i);
				TagParsingSignal = GetAttributeValueParsingSignal;
			} else if(TagParsingSignal == GetAttributeValueParsingSignal) {
			} 
		} else if(str[i] == '>') {
			if(TagParsingSignal == GetNameTagParsingSignal || TagParsingSignal == GetEndTagNameParsingSignal) {
				temp_word = (string) malloc(sizeof(char) * (i-last+1));
				chtml_parser_create_substring(str, temp_word, last, i);
				if(TagParsingSignal == GetEndTagNameParsingSignal) {
					*last_element = chtml_element_stack_pop(el_stack);
					*current_element = chtml_element_stack_last(el_stack);
				} else {
					chtml_parser_add_element(current_element, last_element, el_stack, temp_word);
				}
				free(temp_word);
			} else {
			}
			break;
		} else if(str[i] == '<') {
			// error -> element in an element
		} else {
		
		}
	}
	for(; str[i] && str[i] != '>'; ++i); // if end of a tag is not reached, loop till the end
	*end = i;
	return ret_value;
}

int chtml_parser_parse_content(chtml_element** current_element, chtml_element_stack* el_stack, const char* str, int start, int end) {
	int i, size = 0;
	char last;
	string content = (string) malloc(sizeof(char) * (end-start+1));
	for(i = start; i < end; i++) {
		if(str[i] == '\n' || str[i] == '\t') {
			if(i != start && last != '\n' && last != '\t' && last != ' ') {
				content[size] = ' ';
				size++;
			}
		} else {
			if(str[i] == ' ') {
				if(i != start && last != ' ' && last != '\t' && last != '\n') {
					content[size] = str[i];
					size++;
				}
			}
			else { 
				content[size] = str[i];
				size++;
			}
		}
		last = str[i];
	}
	content[size] = '\0';
	if(size)
		chtml_element_add_content(current_element, content, size);
	free(content);
	return 0;
}

int chtml_parser_parse_string(const char* str, chtml_element** root_element) { 
	*root_element = NULL;
	int i, return_value = 0, line_number = 1, start, end;
	chtml_element_stack el_stack;
	chtml_element* root_bkp = *root_element;
	chtml_element** current_element = root_element;
	chtml_element* last_element = NULL;
	
	chtml_element_stack_init(&el_stack);
	enum ParsingSignal CurrentParsingSignal = NormalParsingSignal;
	enum ParsingSignal LastParsingSignal = NormalParsingSignal; // used for comments
	
	for(i = 0; str[i]; i++) {
		if(str[i] == '<') {
			if(CurrentParsingSignal != CommentParsingSignal) {
				// check for comment
				if(str[i+1] == '!' && str[i+2] == '-' && str[i+3] == '-') {
					LastParsingSignal = CurrentParsingSignal;
					CurrentParsingSignal = CommentParsingSignal;
					i+= 3;
				} else {
					end = i;
					if(end-start > 0 && *current_element != NULL) {
						chtml_parser_parse_content(current_element, &el_stack, str, start, end);
					}
					start = i+1;
					if(chtml_parser_parse_tag(current_element, &last_element, &el_stack, str, start, &end, &line_number)) {
						return_value = 1;
						break;
					}
					CurrentParsingSignal = ContentParsingSignal;
					start = end+1;
					i = end;
					if(!root_bkp && *current_element) { // if root is created store its pointer into a backup for later access
						root_bkp = *current_element;
					}
				}
			}
		} else if(str[i] == '\n') {
			line_number++;
		} else if(str[i] == '-' && CurrentParsingSignal == CommentParsingSignal) {
			if(str[i+1] == '-' && str[i+2] == '>') {
				i += 2;
				CurrentParsingSignal = LastParsingSignal;
				start = i+1;
			}
		}
	}
	*root_element = root_bkp;
	chtml_element_stack_delete(&el_stack);
	return return_value;
}
