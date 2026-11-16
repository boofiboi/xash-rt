import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile

import numpy as np
from PIL import Image

PASSES = [
    (0, "final"),
    (1, "bloom"),
    (2, "volumetric"),
    (3, "denoiser"),
]

NON_EMPTY_PASSES = {"final", "denoiser"}


def compare_images(golden_arr, actual_arr, mae_threshold, pixel_pct_threshold, require_non_empty):
    if golden_arr.shape != actual_arr.shape:
        return False, {
            "reason": f"shape mismatch {actual_arr.shape} != {golden_arr.shape}",
            "mae": 255.0,
            "diff_pct": 100.0,
            "mean": float(actual_arr.mean()),
        }, None

    if require_non_empty and float(actual_arr.mean()) < 0.5:
        return False, {
            "reason": f"image is blank (mean={actual_arr.mean():.3f})",
            "mae": 255.0,
            "diff_pct": 100.0,
            "mean": float(actual_arr.mean()),
        }, None

    diff = np.abs(actual_arr.astype(np.float32) - golden_arr.astype(np.float32))
    mae = float(diff.mean())
    pixel_max_diff = diff.max(axis=2) if diff.ndim == 3 else diff
    diff_pct = float((pixel_max_diff > 12.0).mean() * 100.0)

    ok = mae <= mae_threshold and diff_pct <= pixel_pct_threshold
    reason = "" if ok else f"mae={mae:.2f} (max {mae_threshold}), diff_pct={diff_pct:.2f}% (max {pixel_pct_threshold}%)"
    vis_diff = np.clip(diff * 4.0, 0, 255).astype(np.uint8)
    return ok, {"reason": reason, "mae": mae, "diff_pct": diff_pct, "mean": float(actual_arr.mean())}, vis_diff


def run_self_test():
    base = np.full((64, 64, 3), 100, dtype=np.uint8)
    ok, stats, _ = compare_images(base, base.copy(), 6.0, 12.0, True)
    assert ok and stats["mae"] == 0.0 and stats["diff_pct"] == 0.0

    noisy = base.copy()
    noisy[::2, ::2] = 104
    ok, stats, _ = compare_images(base, noisy, 6.0, 12.0, True)
    assert ok and stats["mae"] > 0.0

    broken = np.full((64, 64, 3), 220, dtype=np.uint8)
    ok, stats, diff = compare_images(base, broken, 6.0, 12.0, True)
    assert not ok and diff is not None

    blank = np.zeros((64, 64, 3), dtype=np.uint8)
    ok, stats, _ = compare_images(base, blank, 6.0, 12.0, True)
    assert not ok and "blank" in stats["reason"]

    ok, _, _ = compare_images(blank, blank.copy(), 6.0, 12.0, False)
    assert ok
    print("Self-test passed")


def build_mod_cfg(cases, out_rel_dir):
    lines = [
        "sv_cheats 1",
        "rt_vsync 0",
        "rt_framegen 0",
        "r_drawviewmodel 0",
        "crosshair 0",
        "cl_showfps 0",
        "con_notifytime 0",
        "scr_conspeed 99999",
    ]
    for case in cases:
        lines.append(f"map {case['map']}")
        lines.append("wait 40")
        lines.append("god")
        lines.append("notarget")
        lines.append("noclip")
        if "pos" in case:
            pos = " ".join(str(v) for v in case["pos"])
            ang = " ".join(str(v) for v in case["ang"]) if "ang" in case else ""
            lines.append(f"setpos {pos} {ang}".strip())
        for cmd in case.get("commands", []):
            lines.append(cmd)
        lines.append(f"wait {case.get('settle_frames', 60)}")
        for target_idx, pass_name in PASSES:
            lines.append(f"_rt_capture_target {target_idx}")
            lines.append("wait 2")
            lines.append(f"snapshot {out_rel_dir}/{case['name']}_{pass_name}.png")
            lines.append("wait 2")
        lines.append("_rt_capture_target 0")
    lines.append("quit")
    return "\n".join(lines) + "\n"


def run_mod(exe_path, game_dir, mod_name, cases, out_dir):
    mod_dir = os.path.join(game_dir, mod_name)
    if not os.path.isdir(mod_dir):
        raise RuntimeError(f"Mod directory not found: {mod_dir}")

    cfg_name = "_game_test.cfg"
    cfg_path = os.path.join(mod_dir, cfg_name)
    out_rel = "../_game_test_out"

    with open(cfg_path, "w", encoding="ascii") as f:
        f.write(build_mod_cfg(cases, out_rel))

    try:
        cmd = [
            exe_path,
            "-windowed",
            "-width", "1280",
            "-height", "720",
            "-game", mod_name,
            "+exec", cfg_name,
        ]
        res = subprocess.run(cmd, cwd=game_dir, timeout=180)
        if res.returncode != 0:
            raise RuntimeError(f"{mod_name} exited with code {res.returncode}")
    finally:
        if os.path.exists(cfg_path):
            os.remove(cfg_path)


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument("--game-dir", default=r"D:\penise\HL1")
    parser.add_argument("--exe", default=None)
    parser.add_argument("--cases", default=os.path.join(script_dir, "cases.json"))
    parser.add_argument("--golden-dir", default=os.path.join(script_dir, "golden"))
    parser.add_argument("--failures-dir", default=os.path.join(script_dir, "failures"))
    parser.add_argument("--mae-threshold", type=float, default=6.0)
    parser.add_argument("--pixel-pct-threshold", type=float, default=12.0)
    parser.add_argument("--bless", action="store_true")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        run_self_test()
        return 0

    exe_path = args.exe or os.path.join(args.game_dir, "rt_bin", "xash3d.exe")
    if not os.path.exists(exe_path):
        print(f"ERROR: executable not found: {exe_path}", file=sys.stderr)
        return 2

    with open(args.cases, "r", encoding="utf-8") as f:
        cases = json.load(f)

    out_dir = os.path.join(args.game_dir, "_game_test_out")
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir, exist_ok=True)

    if args.bless:
        os.makedirs(args.golden_dir, exist_ok=True)
    else:
        if os.path.exists(args.failures_dir):
            shutil.rmtree(args.failures_dir)

    by_mod = {}
    for c in cases:
        by_mod.setdefault(c["game"], []).append(c)

    for mod_name, mod_cases in by_mod.items():
        print(f"Running {mod_name} ({len(mod_cases)} case(s))...")
        run_mod(exe_path, args.game_dir, mod_name, mod_cases, out_dir)

    passed = 0
    failed = 0

    for c in cases:
        for _, pass_name in PASSES:
            filename = f"{c['name']}_{pass_name}.png"
            actual_path = os.path.join(out_dir, filename)
            golden_path = os.path.join(args.golden_dir, filename)

            if not os.path.exists(actual_path):
                print(f"FAIL {filename}: capture missing")
                failed += 1
                continue

            if args.bless:
                shutil.copy2(actual_path, golden_path)
                print(f"BLESS {filename}")
                passed += 1
                continue

            if not os.path.exists(golden_path):
                print(f"FAIL {filename}: golden baseline missing (run with --bless first)")
                failed += 1
                continue

            golden_arr = np.asarray(Image.open(golden_path).convert("RGB"))
            actual_arr = np.asarray(Image.open(actual_path).convert("RGB"))
            ok, stats, vis_diff = compare_images(
                golden_arr,
                actual_arr,
                args.mae_threshold,
                args.pixel_pct_threshold,
                pass_name in NON_EMPTY_PASSES,
            )

            if ok:
                print(f"PASS {filename} (mae={stats['mae']:.2f}, diff={stats['diff_pct']:.2f}%)")
                passed += 1
            else:
                print(f"FAIL {filename}: {stats['reason']}")
                os.makedirs(args.failures_dir, exist_ok=True)
                shutil.copy2(actual_path, os.path.join(args.failures_dir, filename))
                if vis_diff is not None:
                    Image.fromarray(vis_diff).save(
                        os.path.join(args.failures_dir, f"{c['name']}_{pass_name}_diff.png")
                    )
                failed += 1

    print(f"Done! {passed} passed, {failed} failed")
    return 1 if failed > 0 else 0


if __name__ == "__main__":
    sys.exit(main())
