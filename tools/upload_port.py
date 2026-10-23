Import("env")

import sys

from serial.tools import list_ports

ports = env.GetProjectOption("custom_upload_ports", "").split()
present = [p.device for p in list_ports.comports()]

# --upload-port reste prioritaire
if not env.subst("$UPLOAD_PORT"):
    found = next((p for p in ports if p in present), None)
    if found:
        print("Port : " + found)
        env.Replace(UPLOAD_PORT=found)
    elif "upload" in COMMAND_LINE_TARGETS:
        sys.stderr.write("Aucun port connu branche : %s\n" % ", ".join(ports))
        env.Exit(1)
