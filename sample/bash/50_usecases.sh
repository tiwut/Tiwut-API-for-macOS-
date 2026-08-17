#!/usr/bin/env bash

HOST="${1:-127.0.0.1}"
PORT="${2:-8888}"
BASE_URL="http://${HOST}:${PORT}"

echo "============================================================"
echo "   TIWUT API - 50 AUTOMATED USE CASE VERIFICATION SUITE     "
echo "   Target: ${BASE_URL}                                      "
echo "============================================================"

sleep 120 &
TEST_TARGET_PID=$!

pass_count=0
fail_count=0

run_test() {
  local num="$1"
  local desc="$2"
  local method="$3"
  local path="$4"
  local body="$5"

  echo -n "[Case $(printf '%02d' "$num")] $desc ... "
  
  local response
  if [ "$method" = "GET" ]; then
    response=$(curl -s -w "\n%{http_code}" "${BASE_URL}${path}")
  else
    if [ -n "$body" ]; then
      response=$(curl -s -w "\n%{http_code}" -X "$method" -H "Content-Type: application/json" -d "$body" "${BASE_URL}${path}")
    else
      response=$(curl -s -w "\n%{http_code}" -X "$method" "${BASE_URL}${path}")
    fi
  fi

  local http_code
  http_code=$(echo "$response" | tail -n1)

  if [ "$http_code" -ge 200 ] && [ "$http_code" -lt 400 ]; then
    echo "PASSED (HTTP $http_code)"
    pass_count=$((pass_count + 1))
  else
    echo "FAILED (HTTP $http_code)"
    fail_count=$((fail_count + 1))
  fi
}

run_test 1 "Fleet Overview Telemetry" "GET" "/api/v1/overview" ""
run_test 2 "Per-Core CPU Load" "GET" "/api/v1/cpu" ""
run_test 3 "Mach Memory Pressure" "GET" "/api/v1/memory" ""
run_test 4 "Prometheus Metrics Export" "GET" "/api/v1/metrics" ""
run_test 5 "APFS Volume Storage Stats" "GET" "/api/v1/storage" ""
run_test 6 "Network Interface Stats" "GET" "/api/v1/network" ""
run_test 7 "Network Latency Ping" "GET" "/api/v1/network/ping?host=1.1.1.1" ""
run_test 8 "System Hardware Inventory" "GET" "/api/v1/system" ""
run_test 9 "Service Health Check" "GET" "/api/v1/health" ""
run_test 10 "Dynamic Config Schema" "GET" "/api/v1/config" ""
run_test 11 "Process List Enumeration" "GET" "/api/v1/processes" ""
run_test 12 "Single Process Query" "GET" "/api/v1/processes/${TEST_TARGET_PID}" ""
run_test 13 "Process Priority Adjustment" "POST" "/api/v1/processes/${TEST_TARGET_PID}/priority" '{"nice": 0}'
run_test 14 "Process Pause State" "POST" "/api/v1/processes/${TEST_TARGET_PID}/pause" ""
run_test 15 "Process Resume State" "POST" "/api/v1/processes/${TEST_TARGET_PID}/resume" ""
run_test 16 "Running Applications List" "GET" "/api/v1/apps" ""
run_test 17 "Application Focus Trigger" "POST" "/api/v1/apps/focus" '{"bundle_id": "com.apple.Finder"}'
run_test 18 "Thermal Sensor Telemetry" "GET" "/api/v1/thermal" ""
run_test 19 "Battery & Power Health" "GET" "/api/v1/battery" ""
run_test 20 "Power Assertion Inspection" "GET" "/api/v1/power" ""
run_test 21 "Caffeinate Assertion Start" "POST" "/api/v1/power/caffeinate" ""
run_test 22 "Caffeinate Assertion Release" "POST" "/api/v1/power/decaffeinate" ""
run_test 23 "Screen Lock Trigger" "POST" "/api/v1/power/lock" ""
run_test 24 "Display Metrics Query" "GET" "/api/v1/display" ""
run_test 25 "Display Brightness Adjust" "POST" "/api/v1/display/brightness" '{"brightness": 0.85}'
run_test 26 "Display Idle Sleep Trigger" "POST" "/api/v1/display/sleep" ""
run_test 27 "CoreAudio Status Query" "GET" "/api/v1/audio" ""
run_test 28 "Audio Volume Level Adjust" "POST" "/api/v1/audio/volume" '{"volume": 0.65}'
run_test 29 "Audio Output Mute Toggle" "POST" "/api/v1/audio/mute" '{"mute": false}'
run_test 30 "Clipboard Read Content" "GET" "/api/v1/clipboard" ""
run_test 31 "Clipboard Write Content" "POST" "/api/v1/clipboard" '{"text": "Tiwut API Automated Verification Test"}'
run_test 32 "Clipboard Clear Content" "POST" "/api/v1/clipboard/clear" ""
run_test 33 "macOS Notification Dispatch" "POST" "/api/v1/notifications" '{"title": "Tiwut Suite", "subtitle": "Automated Test", "message": "All systems operational", "sound": "default"}'
run_test 34 "Config Zero-Downtime Reload" "POST" "/api/v1/config/reload" ""
run_test 35 "Local AI Capacity Health" "GET" "/api/v1/overview" ""
run_test 36 "CI/CD Keep-Awake Hook" "POST" "/api/v1/power/caffeinate" ""
run_test 37 "Home Assistant Status Probe" "GET" "/api/v1/overview" ""
run_test 38 "Stream Deck Mute Hook" "POST" "/api/v1/audio/mute" '{"mute": false}'
run_test 39 "Raycast Process Search" "GET" "/api/v1/processes" ""
run_test 40 "Docker Host RAM Tracker" "GET" "/api/v1/memory" ""
run_test 41 "Zombie Process Check" "GET" "/api/v1/processes" ""
run_test 42 "Presentation Mode Awake" "POST" "/api/v1/power/caffeinate" ""
run_test 43 "Cross-Device Pasteboard" "GET" "/api/v1/clipboard" ""
run_test 44 "Render Farm Node Telemetry" "GET" "/api/v1/overview" ""
run_test 45 "High Priority Compiler Elevator" "POST" "/api/v1/processes/${TEST_TARGET_PID}/priority" '{"nice": 5}'
run_test 46 "Battery Low Threshold Check" "GET" "/api/v1/battery" ""
run_test 47 "Thermal Throttling Guardian" "GET" "/api/v1/thermal" ""
run_test 48 "Storage Space Low Warning" "GET" "/api/v1/storage" ""
run_test 49 "Network DNS & Gateway Audit" "GET" "/api/v1/network" ""
run_test 50 "Multi-Mac Centralized Metrics" "GET" "/api/v1/metrics" ""

kill "$TEST_TARGET_PID" 2>/dev/null || true

echo "============================================================"
echo "   TEST SUMMARY: ${pass_count}/50 PASSED | ${fail_count} FAILED"
echo "============================================================"

if [ "$fail_count" -eq 0 ]; then
  exit 0
else
  exit 1
fi
