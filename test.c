
#include <stdio.h>
#include <stdlib.h>

static int num_allocs = 0;
void* TEST_ALLOC( size_t sz )
{
	num_allocs++;
	return malloc( sz );
}
void TEST_FREE( void* p )
{
	num_allocs--;
	free( p );
}

#define xt_alloc TEST_ALLOC
#define xt_free TEST_FREE

#include "xmltractor.c"

static xt_Node* root;

#define NODE_STACK_SIZE 256
static xt_Node* chk_node_stack[ NODE_STACK_SIZE ];
int chk_node_stack_pos = 0;

void check_error( const char* code, int line )
{
	fprintf( stderr, "Check on line %d failed:\n\n\t%s\n", line, code );
	exit( 1 );
}
#define CHECK_( x, line ) if( (x) == 0 ) check_error( #x, line )
#define CHECK( x ) CHECK_( x, __LINE__ )

#define PARSE_FAIL( str ) PARSE_FAIL_( str, __LINE__ )
void PARSE_FAIL_( const char* str, int line )
{
	printf( "running test [parse bad input] on line %d\n", line );
	CHECK_( num_allocs == 0 && "something wasn't checked before", line );
	CHECK_( root == NULL, line );
	root = xt_parse( str );
	CHECK_( root == NULL, line );
	CHECK_( num_allocs == 0 && "unfreed memory", line );
}

#define PARSE( str ) PARSE_( str, __LINE__ )
void PARSE_( const char* str, int line )
{
	printf( "running test [parse valid input] on line %d\n", line );
	CHECK_( num_allocs == 0 && "something wasn't checked before", line );
	CHECK_( root == NULL, line );
	root = xt_parse( str );
	CHECK_( root != NULL, line );
	
	chk_node_stack[ 0 ] = root;
	chk_node_stack_pos = 0;
}

void FREE()
{
	CHECK( chk_node_stack_pos == 0 && "error in test" );
	CHECK( root != NULL );
	xt_destroy_node( root );
	root = NULL;
	CHECK( num_allocs == 0 && "unfreed memory" );
}

#define EL( str ) EL_( str, __LINE__ )
void EL_( const char* str, int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	int len = (int) strlen( str );
	if( n->szname != len || memcmp( n->name, str, len ) != 0 )
	{
		fprintf( stderr, "Check on line %d failed: EL(\"%s\") [length=%d]\n", line, str, len );
		fprintf( stderr, "- node name actually was \"%.*s\" [length=%d]\n", n->szname, n->name, n->szname );
		exit( 1 );
	}
}
#define CONTENT( str ) CONTENT_( str, __LINE__ )
void CONTENT_( const char* str, int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	int len = (int) strlen( str );
	if( n->szcontent != len || memcmp( n->content, str, len ) != 0 )
	{
		fprintf( stderr, "Check on line %d failed: CONTENT(\"%s\") [length=%d]\n", line, str, len );
		fprintf( stderr, "- node content actually was \"%.*s\" [length=%d]\n", n->szcontent, n->content, n->szcontent );
		exit( 1 );
	}
}
#define HEADER( str ) HEADER_( str, __LINE__ )
void HEADER_( const char* str, int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	int len = (int) strlen( str );
	if( n->szheader != len || memcmp( n->header, str, len ) != 0 )
	{
		fprintf( stderr, "Check on line %d failed: HEADER(\"%s\") [length=%d]\n", line, str, len );
		fprintf( stderr, "- node header actually was \"%.*s\" [length=%d]\n", n->szheader, n->header, n->szheader );
		exit( 1 );
	}
}
#define ATTRNUM( num ) ATTRNUM_( num, __LINE__ )
void ATTRNUM_( int num, int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	if( n->numattribs != num )
	{
		fprintf( stderr, "Check on line %d failed: ATTRNUM(%d)\n", line, num );
		fprintf( stderr, "- node attribute count actually was %d\n", n->numattribs );
		exit( 1 );
	}
}
#define CHNUM( num ) CHNUM_( num, __LINE__ )
void CHNUM_( int num, int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	if( n->numchildren != num )
	{
		fprintf( stderr, "Check on line %d failed: CHNUM(%d)\n", line, num );
		fprintf( stderr, "- node child count actually was %d\n", n->numchildren );
		exit( 1 );
	}
}
#define PUSH PUSH_( __LINE__ );
void PUSH_( int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	CHECK_( n->firstchild, line );
	CHECK_( n->firstchild->parent == n, line );
	chk_node_stack_pos++;
	CHECK_( chk_node_stack_pos < NODE_STACK_SIZE, line );
	chk_node_stack[ chk_node_stack_pos ] = n->firstchild;
}
#define POP POP_( __LINE__ );
void POP_( int line )
{
	CHECK_( chk_node_stack_pos > 0, line );
	chk_node_stack_pos--;
}
#define NOCH NOCH_( __LINE__ );
void NOCH_( int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	CHECK_( n->firstchild == NULL, line );
}
#define NEXT NEXT_( __LINE__ );
void NEXT_( int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	CHECK_( n->sibling, line );
	chk_node_stack[ chk_node_stack_pos ] = n->sibling;
}
#define END END_( __LINE__ );
void END_( int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	CHECK_( n->sibling == NULL, line );
}
#define NOATTR NOATTR_( __LINE__ );
void NOATTR_( int line )
{
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	CHECK_( n->attribs == NULL, line );
}
#define ATTR( i, nm, val ) ATTR_( i, nm, val, __LINE__ )
void ATTR_( int i, const char* nm, const char* val, int line )
{
	xt_Attrib* attr;
	xt_Node* n = chk_node_stack[ chk_node_stack_pos ];
	int szname = (int) strlen( nm );
	int szval = (int) strlen( val );
	CHECK_( i >= 0 && i < n->numattribs, line );
	attr = &n->attribs[ i ];
	if( attr->szname != szname || memcmp( attr->name, nm, szname ) != 0 ||
		attr->szvalue != szval || memcmp( attr->value, val, szval ) != 0 )
	{
		fprintf( stderr, "Check on line %d failed: ATTR(%d,\"%s\",\"%s\")\n", line, i, nm, val );
		fprintf( stderr, "- node attribute actually was \"%.*s\"[%d]=\"%.*s\"[%d]\n",
			attr->szname, attr->name, attr->szname, attr->szvalue, attr->value, attr->szvalue );
		exit( 1 );
	}
}


int main()
{
	puts( "-- XML Tractor unit tests --" );
	
	PARSE( "<a></a>" );
	FREE();
	
	PARSE_FAIL( "<" );
	PARSE_FAIL( "a" );
	PARSE_FAIL( "<a" );
	PARSE_FAIL( ">" );
	PARSE_FAIL( "/>" );
	PARSE_FAIL( "<a></b>" );
	PARSE_FAIL( "<a x=\"y'></a>" );
	PARSE_FAIL( "<a x='y\"></a>" );
	
	PARSE( "<a></a>" );
		EL( "a" ); CONTENT( "" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
	FREE();
	PARSE( "<a>\n</a>" );
		EL( "a" ); CONTENT( "\n" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
	FREE();
	
	PARSE( "<a><b></b></a>" );
		EL( "a" ); CONTENT( "<b></b>" ); ATTRNUM( 0 ); CHNUM( 1 );
		PUSH
			EL( "b" ); CONTENT( "" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
			END
		POP
	FREE();
	
	/* skip declaration */
	PARSE( "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><a></a>" );
		EL( "a" ); CONTENT( "" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
	FREE();
	
	PARSE( "<wa attrname=\"attrval\"></wa>" );
		EL( "wa" ); CONTENT( "" ); ATTRNUM( 1 ); CHNUM( 0 ); NOCH ATTR( 0, "attrname", "attrval" );
	FREE();
	
	PARSE( "<a>b<c>d</c>e<c>f</c>g</a>" );
		EL( "a" ); CONTENT( "b<c>d</c>e<c>f</c>g" ); HEADER( "<a>" ); ATTRNUM( 0 ); CHNUM( 2 ); NOATTR
		PUSH
			EL( "c" ); CONTENT( "d" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
			NEXT
			EL( "c" ); CONTENT( "f" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
			END
		POP
	FREE();
	
	PARSE( "<a />" );
		EL( "a" ); CONTENT( "" ); HEADER( "<a />" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
	FREE();
	PARSE( "<wa2 abc='def' ghi=\"jklmn\" />" );
		EL( "wa2" ); CONTENT( "" ); ATTRNUM( 2 ); CHNUM( 0 ); NOCH ATTR( 0, "abc", "def" ); ATTR( 1, "ghi", "jklmn" );
	FREE();
	PARSE( "<a/>" );
		EL( "a" ); CONTENT( "" ); HEADER( "<a/>" ); ATTRNUM( 0 ); CHNUM( 0 ); NOCH NOATTR
	FREE();
	
	puts( "-- success --" );
	return 0;
}
