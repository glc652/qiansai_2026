# Upper-arm MQTT payloads

Realtime topic:

```text
rehab/upper/realtime
```

Health topic:

```text
rehab/upper/health
```

## Pages

| Enum | Page id | Payload |
| --- | ---: | --- |
| `UPPER_PAGE_HOME` | 0 | home payload |
| `UPPER_PAGE_PREP` | 1 | home payload |
| `UPPER_PAGE_ACTION_SELECT` | 2 | home payload |
| `UPPER_PAGE_TRAINING` | 3 | training payload |
| `UPPER_PAGE_RESULT` | 4 | result payload |
| `UPPER_PAGE_SETTINGS` | 5 | home payload |
| `UPPER_PAGE_HISTORY` | 6 | home payload |
| `UPPER_PAGE_ANALYTICS` | 7 | home payload |
| `UPPER_PAGE_LINK_LOST` | 8 | home payload |
| `UPPER_PAGE_FREE_TRAINING` | 9 | training payload |
| `UPPER_PAGE_FREE_RESULT` | 10 | result payload |

## Realtime payloads

All non-training and non-result pages use the home payload:

```text
t,pg,ue,ub,wb,uba,wba
```

Training and free-training use the shortest realtime training payload:

```text
pg,a,n,ang,e,ab,sh
```

Result and free-result use:

```text
t,pg,a,n,g,s,m,av,em,e,ab,sh
```

## Field map

| Key | Meaning |
| --- | --- |
| `t` | RTOS tick |
| `pg` | page id |
| `ue` | upper page event |
| `ub` | upper link state |
| `wb` | wrist link state |
| `uba` | upper battery percent |
| `wba` | wrist battery percent |
| `a` | action id |
| `n` | completed reps |
| `g` | target reps |
| `ang` | current training angle |
| `s` | score |
| `m` | max angle |
| `av` | average peak angle |
| `em` | EMG rate |
| `e` | elbow flexion angle |
| `ab` | arm abduction angle |
| `sh` | shoulder lift angle |

## Current test

The current firmware test enables:

```c
#define ESP8266_LONGEST_PAYLOAD_SIM_TEST  (1U)
```

This sends the longest realtime payload, currently the training payload, every `ESP8266_UPLOAD_PERIOD_MS`.
