#!/bin/bash

# Define the endpoint URL
URL="http://localhost:8000/smart_query"

# Define the JSON payload
PAYLOAD=$(cat <<EOF
{
  "query": "why the sky is blue",
  "k": 5
}
EOF
)

# Make the HTTP GET request
curl -X GET "$URL" -H "Content-Type: application/json" -d "$PAYLOAD"