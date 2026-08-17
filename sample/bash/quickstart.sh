#!/usr/bin/env bash

set -e

echo "Building Tiwut API Core, CFeel, C++ and Web Samples..."
make all
make samples

echo "Starting Tiwut API Server on background port 8888..."
./bin/tiwut-api-server &
SERVER_PID=$!

sleep 1

echo "Testing server overview endpoint..."
curl -s http://127.0.0.1:8888/api/v1/overview | head -n 30

echo "Running 50 automated use cases..."
bash sample/bash/50_usecases.sh

echo "Stopping background server (PID: $SERVER_PID)..."
kill $SERVER_PID 2>/dev/null || true

echo "Quickstart completed successfully!"
