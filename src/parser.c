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
		content = (string) malloc(file_size);
		fread(content, sizeof(char), file_size, file);
		return_value = chtml_parser_parse_string(content, root_element);
		free(content);
		fclose(file);
	}
	return return_value;
}

// parses html/xml/xhtml content

int chtml_parser_parse_tag(const char* str, int start, int end) {
	int i;
	printf("TAG: ");
	for(i = start; i <= end; i++) {
		putc(str[i], stdout);
	}
	printf("\n");
	return 0;
}

int chtml_parser_parse_content(const char* str, int start, int end) {
	int i;
	printf("CONTENT: [%d] -> ", end-start);
	for(i = start; i <= end; i++) {
		putc(str[i], stdout);
	}
	if(str[i-1] != '\n')
		printf("\n");
	return 0;
}

int chtml_parser_parse_string(const char* str, chtml_element** root_element) { 
	int i, return_value = 0, line_number = 1, start, end;
	CurrentParsingSignal = NormalParsingSignal;
	for(i = 0; str[i]; i++) {
		if(str[i] == '<') {
			if(CurrentParsingSignal == ContentParsingSignal) {
				end = i-1;
				if(end-start > 0)
					chtml_parser_parse_content(str, start, end);
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
			chtml_parser_parse_tag(str, start, end);
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
				CurrentParsingSignal = NormalParsingSignal;
				++i;
				continue;
			} else {
				fprintf(stderr, "Invalid character '?' at line %d\n", line_number);
				return_value = 1;
				break;
			}
		} else if(str[i] == '\n') {
			line_number++;
		}
	}
	return return_value;
}
