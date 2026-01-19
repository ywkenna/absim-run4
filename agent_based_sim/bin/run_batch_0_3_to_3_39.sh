#!/usr/bin/env bash
set -u

BIN_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$BIN_DIR"

DATA_ROOT="../data/2026SHELTERSEEK/scenario"
SCEN_DIR="${DATA_ROOT}"   # json이 있는 폴더
LOG_ROOT="./batch_logs_0_3_to_3_39"
mkdir -p "$LOG_ROOT"

EVOPT="${1:-1}"  # 두 번째 인자 없으면 1

echo "[INFO] start at: $(date '+%F %T')"
echo "[INFO] BIN_DIR=$BIN_DIR"
echo "[INFO] SCEN_DIR=$SCEN_DIR"
echo "[INFO] LOG_ROOT=$LOG_ROOT"
echo "[INFO] EVOPT=$EVOPT"
echo

# 0_3 ~ 3_39
for s in 0 1 2 3; do
  for p in $(seq 0 39); do
    # 0_0 ~ 0_2 스킵
    if [ "$s" -eq 0 ] && [ "$p" -lt 3 ]; then
      continue
    fi

    base="2026shelterseek_${s}_${p}"
    cfg="${SCEN_DIR}/${base}_mpdua_event.json"

    # json 존재 확인
    if [ ! -f "$cfg" ]; then
      echo "[WARN] missing json: $cfg" | tee -a "${LOG_ROOT}/_missing_json.log"
      continue
    fi

    # 케이스별 로그 파일
    log="${LOG_ROOT}/${base}.log"

    echo "============================================================" | tee -a "$log"
    echo "[CASE] ${base}  (CFG=$cfg, EVOPT=$EVOPT)" | tee -a "$log"
    echo "[TIME] $(date '+%F %T')" | tee -a "$log"

    # 실행 (실패해도 다음 케이스로 진행)
    ./run_kjuj_event_additional "$cfg" "$EVOPT" >> "$log" 2>&1
    rc=$?

    echo "[DONE] ${base} rc=${rc} at $(date '+%F %T')" | tee -a "$log"

    if [ "$rc" -ne 0 ]; then
      echo "${base} rc=${rc}" >> "${LOG_ROOT}/_failed_cases.txt"
    else
      echo "${base}" >> "${LOG_ROOT}/_ok_cases.txt"
    fi

  done
done

echo
echo "[INFO] finished at: $(date '+%F %T')"
echo "[INFO] ok list    : ${LOG_ROOT}/_ok_cases.txt"
echo "[INFO] failed list : ${LOG_ROOT}/_failed_cases.txt"
