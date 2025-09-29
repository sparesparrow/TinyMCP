#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 3 ]]; then
  echo "Usage: $0 <artifact_path> <s3_bucket> <s3_prefix>"
  exit 1
fi

ARTIFACT="$1"
BUCKET="$2"
PREFIX="$3"

aws s3 cp "$ARTIFACT" "s3://${BUCKET}/${PREFIX}/"
echo "Uploaded $ARTIFACT to s3://${BUCKET}/${PREFIX}/"

