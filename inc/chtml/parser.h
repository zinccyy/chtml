#ifndef __CHTML_PARSER_H__
#define __CHTML_PARSER_H__

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <chtml/element_stack.h>

extern int errno;

int chtml_parser_parse_file(const char* , chtml_element** );
int chtml_parser_parse_string(const char* , chtml_element** );
int chtml_parser_parse_tag(chtml_element** , chtml_element**, chtml_element_stack*, const char*, int, int*, int*);
int chtml_parser_add_element(chtml_element**, chtml_element**, chtml_element_stack*, string );
int chtml_parser_parse_content(chtml_element**, chtml_element**, chtml_element_stack*, const char*, int, int, char);

#endif
