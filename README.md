XML Tractor
================================
Goes through all that shit so you don't have to.

It's a lightweight C library for in-place XML parsing (it means that you must keep the input string available for the lifetime of the tree).

Useful for write-and-forget tools that are meant to decrypt this awful file format because someone made the poor choice and used it in the file.

Usage:
<pre>
xt_Node* root = xt_parse( input_string );

// use it here

xt_destroy_node( root );
</pre>


How does this compare to other XML parsers out there?
---------------------------------
Source files are 6 kB big. In comparison, RapidXML has 141 kB of source.
It's quite fast too: time it takes to parse big files in XML Tractor vs RapidXML = 3:2.

Known issues:
---------------------------------
This parser ignores hints and might not parse UTF-8 perfectly (it definitely doesn't try to validate it). Since people rarely use anything above the ASCII character set in most non-HTML XML files, this should be enough. If that proves to be false and you've found the source of a problem, do let me know and I'll see what I can do.
No detailed errors will be thrown, the parser will try to recover the data if it can and leave unrecoverable parts empty.

Random info
---------------------------------
This parser is tested on a rather big (259 kB) COLLADA .dae file exported from Blender and on a 113 MB big test XML file generated with this app: http://www.xml-benchmark.org/generator.html
A 113 MB XML file was parsed in 1 second on a rather old PC (CPU is somewhere from 2005).
More people should use linear / command-based file formats. That tree in your favorite file format - you most probably don't need it at all.

TODO
---------------------------------
Optimize for instruction cache.
Add support for comments and CDATA.
Test UTF-8 support.
Add parsing of <! entities to safely ignore them.