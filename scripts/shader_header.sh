#!/bin/sh

OUTPUT=$1

VERT_FILE=./src/shaders/simple.vert
FRAG_FILE=./src/shaders/simple.frag


VERT_PRGM=$(cat ${VERT_FILE} | sed -E ':a;N;$!ba;s/\r{0,1}\n/\\n/g' | sed 's/"/\\"/g')
FRAG_PRGM=$(cat ${FRAG_FILE} | sed -E ':a;N;$!ba;s/\r{0,1}\n/\\n/g' | sed 's/"/\\"/g')

echo "static const char *vertex_shader_text = \"""${VERT_PRGM}""\\n\";" > ${OUTPUT}
echo "static const char *fragment_shader_text = \"""${FRAG_PRGM}""\\n\";" >> ${OUTPUT}
