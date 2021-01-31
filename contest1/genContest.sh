#!/bin/bash

cd include
for header in *; do
	name='src/'
	source=${header//.h}
	source+='.c'
	name+=${source}
	cd ..
	{ cat include/${header} & tail -n +2 ${name} ; } > contest/${source}
	cd include
done
