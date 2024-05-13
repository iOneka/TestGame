import datetime
import os
import shutil
import sys
import subprocess

sys.path.insert(1, os.path.join(sys.path[0], '..\pylibs'))
from rich.console import Console

console = Console(log_time=False, log_path=False)
workspace_dir = os.path.abspath(os.path.join(os.path.curdir, r'../../'))

console.print(":exclamation::exclamation::exclamation: Do you really want to fully clean workspace?")
console.print()
input("Press Enter to continue or close console window to quit...")

with console.status(" Undo changed, added, moved and copied files...") as undo_status:
    cm_undo_command = "cm undo . -r"
    cm_undo_process = subprocess.run(cm_undo_command, capture_output=True, text=True, cwd=workspace_dir)
    cm_undo_output = cm_undo_process.stdout

    console.log(":thumbs_up: Undo changed, added, moved and copied files")

with console.status(" Deleting all private, hidden and ignored files...") as find_branches_status:
    cm_status_command = "cm status --private --ignored --cutignored --compact --short"
    cm_status_process = subprocess.run(cm_status_command, capture_output=True, text=True, cwd=workspace_dir)
    cm_status_output = cm_status_process.stdout

    for path in cm_status_output.split('\n'):
        if os.path.isfile(path):
            os.remove(path)
        elif os.path.isdir(path):
            shutil.rmtree(path)

    console.log(":thumbs_up: Deleting all private, hidden and ignored files")
