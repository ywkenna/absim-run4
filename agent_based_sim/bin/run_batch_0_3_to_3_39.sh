#!/bin/bash

DATA_DIR="../data/2026SHELTERSEEK/scenario"
LOG_DIR="./logs"

mkdir -p "$LOG_DIR"

for state in {1..3}
do
  for policy in {0..2}
  do
    CFG="$DATA_DIR/2026shelterseek_${state}_${policy}_mpdua_event.json"

    if [ ! -f "$CFG" ]; then
      echo "[SKIP] missing $CFG"
      continue
    fi

    echo "[RUN] state=$state policy=$policy $(date)"
    ./run_kjuj_event_additional "$CFG" 1 \
      > "$LOG_DIR/run_${state}_${policy}.log" 2>&1

    echo "[DONE] state=$state policy=$policy $(date)"
  done
done

echo "ALL DONE $(date)"
