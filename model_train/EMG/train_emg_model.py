import argparse
import json
from pathlib import Path

import joblib
import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.model_selection import train_test_split


LABELS = ["relax", "weak", "active"]
FEATURE_NAMES = [
    "raw_over_baseline",
    "raw_delta_over_baseline",
    "drop_over_baseline",
    "rectified_over_baseline",
    "envelope_over_baseline",
    "rms_over_baseline",
]


def parse_args():
    parser = argparse.ArgumentParser(description="Train relax/weak/active EMG classifier.")
    parser.add_argument("csv", nargs="+", help="CSV files collected by collect_emg_data.py.")
    parser.add_argument("--out-dir", default="outputs", help="Output directory.")
    parser.add_argument("--trees", type=int, default=80, help="Random forest tree count.")
    parser.add_argument("--max-depth", type=int, default=5, help="Tree max depth.")
    parser.add_argument("--test-size", type=float, default=0.25, help="Validation split ratio.")
    parser.add_argument("--seed", type=int, default=42, help="Random seed.")
    return parser.parse_args()


def make_features(df):
    baseline = df["baseline_x10"].astype(float).replace(0, np.nan)
    raw_x10 = df["raw"].astype(float) * 10.0
    x = pd.DataFrame(
        {
            "raw_over_baseline": raw_x10 / baseline,
            "raw_delta_over_baseline": (raw_x10 - baseline) / baseline,
            "drop_over_baseline": df["drop_x10"].astype(float) / baseline,
            "rectified_over_baseline": df["rectified_x10"].astype(float) / baseline,
            "envelope_over_baseline": df["envelope_x10"].astype(float) / baseline,
            "rms_over_baseline": df["rms_x10"].astype(float) / baseline,
        }
    )
    return x.replace([np.inf, -np.inf], np.nan).fillna(0.0)


def load_dataset(paths):
    frames = []
    for path in paths:
        frame = pd.read_csv(path)
        frame["source_file"] = str(path)
        frames.append(frame)
    df = pd.concat(frames, ignore_index=True)

    required = {"raw", "baseline_x10", "drop_x10", "rectified_x10", "envelope_x10", "rms_x10", "label"}
    missing = sorted(required - set(df.columns))
    if missing:
        raise ValueError(f"CSV missing columns: {missing}")

    df = df[df["label"].isin(LABELS)].copy()
    if "calibrated" in df.columns:
        df = df[df["calibrated"].astype(int) != 0].copy()
    if df.empty:
        raise ValueError("No valid rows after filtering labels/calibrated.")
    return df


def export_c_model(model, out_c, out_h):
    estimators = model.estimators_
    nodes = []
    roots = []

    for estimator in estimators:
        tree = estimator.tree_
        root_base = len(nodes)
        roots.append(root_base)
        for i in range(tree.node_count):
            if tree.children_left[i] == tree.children_right[i]:
                values = tree.value[i][0]
                cls = int(np.argmax(values))
                nodes.append((-1, -1, 0, 0.0, cls))
            else:
                nodes.append(
                    (
                        root_base + int(tree.children_left[i]),
                        root_base + int(tree.children_right[i]),
                        int(tree.feature[i]),
                        float(tree.threshold[i]),
                        0,
                    )
                )

    out_h.write_text(
        """#ifndef EMG_PARTICIPATION_MODEL_H
#define EMG_PARTICIPATION_MODEL_H

#include <stdint.h>

typedef enum
{
  EMG_PARTICIPATION_RELAX = 0,
  EMG_PARTICIPATION_WEAK = 1,
  EMG_PARTICIPATION_ACTIVE = 2
} EmgParticipation_t;

typedef struct
{
  uint16_t raw;
  int32_t baseline_x10;
  int32_t drop_x10;
  int32_t rectified_x10;
  int32_t envelope_x10;
  int32_t rms_x10;
} EmgParticipationInput_t;

EmgParticipation_t EmgParticipationModel_Predict(const EmgParticipationInput_t *input, uint16_t *confidence_x1000);
const char *EmgParticipationModel_LabelName(EmgParticipation_t label);

#endif
""",
        encoding="utf-8",
    )

    left = ", ".join(str(n[0]) for n in nodes)
    right = ", ".join(str(n[1]) for n in nodes)
    feature = ", ".join(str(n[2]) for n in nodes)
    threshold = ", ".join("0.0f" if n[3] == 0.0 else f"{n[3]:.9g}f" for n in nodes)
    klass = ", ".join(str(n[4]) for n in nodes)
    roots_text = ", ".join(str(r) for r in roots)

    out_c.write_text(
        f"""#include "emg_participation_model.h"

#define EMG_TREE_COUNT ({len(roots)}U)
#define EMG_CLASS_COUNT (3U)
#define EMG_FEATURE_COUNT (6U)
#define EMG_NODE_COUNT ({len(nodes)}U)

static const int16_t kLeft[EMG_NODE_COUNT] = {{{left}}};
static const int16_t kRight[EMG_NODE_COUNT] = {{{right}}};
static const uint8_t kFeature[EMG_NODE_COUNT] = {{{feature}}};
static const float kThreshold[EMG_NODE_COUNT] = {{{threshold}}};
static const uint8_t kClass[EMG_NODE_COUNT] = {{{klass}}};
static const uint16_t kRoot[EMG_TREE_COUNT] = {{{roots_text}}};
static const char *kLabelNames[EMG_CLASS_COUNT] = {{"relax", "weak", "active"}};

static uint8_t PredictTree(uint16_t node, const float features[EMG_FEATURE_COUNT])
{{
  while (kLeft[node] >= 0)
  {{
    if (features[kFeature[node]] <= kThreshold[node])
    {{
      node = (uint16_t)kLeft[node];
    }}
    else
    {{
      node = (uint16_t)kRight[node];
    }}
  }}
  return kClass[node];
}}

EmgParticipation_t EmgParticipationModel_Predict(const EmgParticipationInput_t *input, uint16_t *confidence_x1000)
{{
  float features[EMG_FEATURE_COUNT];
  uint16_t votes[EMG_CLASS_COUNT] = {{0}};
  uint16_t best_votes = 0U;
  uint8_t best_class = 0U;

  if ((input == 0) || (input->baseline_x10 <= 0))
  {{
    if (confidence_x1000 != 0) {{ *confidence_x1000 = 0U; }}
    return EMG_PARTICIPATION_RELAX;
  }}

  features[0] = ((float)input->raw * 10.0f) / (float)input->baseline_x10;
  features[1] = (((float)input->raw * 10.0f) - (float)input->baseline_x10) / (float)input->baseline_x10;
  features[2] = (float)input->drop_x10 / (float)input->baseline_x10;
  features[3] = (float)input->rectified_x10 / (float)input->baseline_x10;
  features[4] = (float)input->envelope_x10 / (float)input->baseline_x10;
  features[5] = (float)input->rms_x10 / (float)input->baseline_x10;

  for (uint16_t i = 0U; i < EMG_TREE_COUNT; i++)
  {{
    votes[PredictTree(kRoot[i], features)]++;
  }}

  for (uint8_t i = 0U; i < EMG_CLASS_COUNT; i++)
  {{
    if (votes[i] > best_votes)
    {{
      best_votes = votes[i];
      best_class = i;
    }}
  }}

  if (confidence_x1000 != 0)
  {{
    *confidence_x1000 = (uint16_t)(((uint32_t)best_votes * 1000U) / EMG_TREE_COUNT);
  }}
  return (EmgParticipation_t)best_class;
}}

const char *EmgParticipationModel_LabelName(EmgParticipation_t label)
{{
  uint8_t index = (uint8_t)label;
  if (index >= EMG_CLASS_COUNT)
  {{
    return "unknown";
  }}
  return kLabelNames[index];
}}
""",
        encoding="utf-8",
    )


def main():
    args = parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    df = load_dataset([Path(p) for p in args.csv])
    x = make_features(df)
    y = df["label"].map({label: i for i, label in enumerate(LABELS)}).astype(int)

    x_train, x_test, y_train, y_test = train_test_split(
        x, y, test_size=args.test_size, random_state=args.seed, stratify=y
    )

    model = RandomForestClassifier(
        n_estimators=args.trees,
        max_depth=args.max_depth,
        random_state=args.seed,
        class_weight="balanced",
        n_jobs=-1,
    )
    model.fit(x_train, y_train)
    y_pred = model.predict(x_test)

    report = {
        "labels": LABELS,
        "feature_names": FEATURE_NAMES,
        "rows": int(len(df)),
        "label_counts": df["label"].value_counts().to_dict(),
        "accuracy": float((y_pred == y_test).mean()),
        "classification_report": classification_report(y_test, y_pred, target_names=LABELS, output_dict=True),
        "confusion_matrix": confusion_matrix(y_test, y_pred).tolist(),
    }

    joblib.dump(model, out_dir / "emg_participation_rf.joblib")
    (out_dir / "emg_participation_report.json").write_text(
        json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8"
    )
    export_c_model(
        model,
        out_dir / "emg_participation_model.c",
        out_dir / "emg_participation_model.h",
    )

    print(f"rows: {report['rows']}")
    print(f"label_counts: {report['label_counts']}")
    print(f"accuracy: {report['accuracy']:.4f}")
    print(f"outputs: {out_dir.resolve()}")


if __name__ == "__main__":
    main()
