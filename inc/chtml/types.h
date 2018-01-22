#ifndef __CHTML_TYPES_H__
#define __CHTML_TYPES_H__

typedef char* string;

enum {
	NormalParsingSignal,
	ElementTagParsingSignal,
	ElementAttributeParsingSignal,
	CommentParsingSignal,
	ScriptParsingSignal, // treated as a comment for now
	ContentParsingSignal,
	DoctypeParsingSignal,
	XmlEncodingParsingSignal
} CurrentParsingSignal;

#endif
