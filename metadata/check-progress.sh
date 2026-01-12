#!/bin/bash
# Check metadata progress across all ezines

ARCHIVES_DIR="../archives"

echo "=== Metadata Progress Check ==="
echo ""

# Count total ezines
total=$(ls -1 "$ARCHIVES_DIR" | wc -l | tr -d ' ')
echo "Total ezines in archives/: $total"

# Count ezines with metadata
with_meta=$(find "$ARCHIVES_DIR" -name "meta.yaml" 2>/dev/null | wc -l | tr -d ' ')
echo "Ezines with metadata: $with_meta"

# Calculate percentage
if [ "$total" -gt 0 ]; then
  percent=$((with_meta * 100 / total))
  echo "Progress: $percent%"
fi

echo ""
echo "=== Ezines WITH metadata ==="
find "$ARCHIVES_DIR" -name "meta.yaml" -exec dirname {} \; | xargs -I {} basename {} | sort

echo ""
echo "=== Ezines MISSING metadata (sample of 20) ==="
# Find ezines without metadata
comm -23 \
  <(ls -1 "$ARCHIVES_DIR" | sort) \
  <(find "$ARCHIVES_DIR" -name "meta.yaml" -exec dirname {} \; | xargs -I {} basename {} | sort) \
  | head -20

echo ""
echo "=== To create metadata for an ezine ==="
echo "1. cd archives/phrack/"
echo "2. cp ../metadata/TEMPLATE.yaml meta.yaml"
echo "3. Read files from ./ (current directory)"
echo "4. Fill in meta.yaml following ../metadata/INSTRUCTIONS.md"
