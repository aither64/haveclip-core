#!/bin/sh

echo "Generating header for git version" $1 $2
HEADER_FILE="$2"

if [ -z "$2" ]; then
    GIT_HEADER="$1/src/git_version.h"
fi

GIT_COMMIT="`git -C \"$1\" rev-parse HEAD`"
if grep --quiet "$GIT_COMMIT" "$HEADER_FILE" ; then
    echo "No need to generate new $HEADER_FILE"
    exit 0;
fi

echo "git commit is:" $GIT_COMMIT

cat <<EOF > "$HEADER_FILE"
#ifndef GIT_VERSION_H
#define GIT_VERSION_H

#define GIT_CURRENT_SHA1 "$GIT_COMMIT"

#endif // GIT_VERSION_H

EOF
