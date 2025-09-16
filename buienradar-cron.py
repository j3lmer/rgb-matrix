#!/usr/bin/env python3
from datetime import datetime
import time
import json
import subprocess
import serial
import re

SERIAL_PORT = "/dev/ttyACM1"   # adjust if different
BAUDRATE = 9600


def get_weather():
    """Run buienradar CLI and return parsed Python object."""
    result = subprocess.run(
        ["/usr/bin/python3", "-m", "buienradar",
         "--latitude=53.3222400",
         "--longitude=53.3222400",
         "--timeframe=30"],
        capture_output=True,
        text=True,
        check=True
    )

    raw = result.stdout
    sanitized = sanitize_buienradar_output(raw)
    return json.loads(sanitized)


def sanitize_buienradar_output(output: str) -> str:
    # Remove all datetime(...) calls
    output = re.sub(r"datetime\.datetime\([^)]+\)", '"datetime"', output)
    # Replace single quotes with double quotes
    output = output.replace("'", '"')
    # Replace True/False/None with lowercase for JSON
    output = output.replace("True", "true").replace(
        "False", "false").replace("None", "null")
    return output


def map_condition(cond: str) -> str:
    """Map Buienradar condition string to Arduino command."""
    cond = cond.lower()
    if "sun" in cond or "clear" in cond:
        return "SUN"
    elif "cloud" in cond:
        return "CLOUD"
    elif "rain" in cond:
        return "RAIN"
    return "UNKNOWN"


def main():
    weather = get_weather()
    forecast = weather["data"]["forecast"][0]  # first slot
    condition = forecast["condition"]["condition"]

    cmd = map_condition(condition)
    log_line = f"[{datetime.now()}] Condition: {condition} -> {cmd}"
    print(log_line)

    # open serial
    with serial.Serial(SERIAL_PORT, BAUDRATE, timeout=2) as ser:
        time.sleep(2)  # wait for Arduino reset
        ser.write((cmd + "\n").encode("utf-8"))
        # ser.write(("SUN\n").encode("utf-8"))

        # try to read Arduino echo
        reply = ser.readline().decode(errors="ignore").strip()
        if reply:
            print(f"[{datetime.now()}] Arduino replied: {reply}")


if __name__ == "__main__":
    main()
