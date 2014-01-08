#!/bin/bash

rm statsPrefetch
for i in $(seq 1 28); do
	./simulator | grep ^Stat | tail -n 1 | cut -d' ' -f 3 >> statsPrefetch
done
