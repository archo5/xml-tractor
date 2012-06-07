
/*
	XML Tractor [v1.0]
	- goes through all that shit so you don't have to
	Copyright � 2012 Arv�ds Kokins
	More info on http://code.google.com/p/xml-tractor/
*/

#pragma once

#ifdef  __cplusplus
extern "C"{
#endif


#include <malloc.h>


#define xt_alloc malloc
#define xt_free free


typedef struct xt_Attrib xt_Attrib;
typedef struct xt_Node xt_Node;

struct xt_Attrib
{
	char*	name;
	char*	value;
	int		szname;
	int		szvalue;
};

struct xt_Node
{
	xt_Node*	parent;
	xt_Node*	firstchild;
	xt_Node*	sibling;
	char*		header;
	char*		content;
	char*		name;
	xt_Attrib*	attribs;
	int			numchildren;
	int			szheader;
	int			szcontent;
	int			szname;
	int			numattribs;
};

xt_Node* xt_parse( const char* data );
void xt_destroy_node( xt_Node* root );

xt_Node* xt_find_child( xt_Node* node, const char* name );
xt_Attrib* xt_find_attrib( xt_Node* node, const char* name );


#ifdef  __cplusplus
}
#endif
