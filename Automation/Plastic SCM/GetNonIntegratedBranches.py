import datetime
import os
import sys
import subprocess
from datetime import datetime

sys.path.insert(1, os.path.join(sys.path[0], '..\pylibs'))
from rich.console import Console
from rich.table import Table

console = Console(log_time=False, log_path=False)

with console.status(" Fetching all branches...") as find_branches_status:
    cm_find_branches_command = "cm find branches \"where date >= '2023/10/15'\" --format=\"{name} -> {owner} -> {date}\" --nototal"
    cm_find_branches_process = subprocess.run(cm_find_branches_command, capture_output=True, text=True)
    cm_find_branches_output = cm_find_branches_process.stdout

    lines = cm_find_branches_output.strip().split("\n")
    all_branches_with_owner = {}

    for line in lines:
        branch, owner, date = line.split(" -> ")
        all_branches_with_owner[branch] = {'owner': owner, 'date': date}

    console.log(":thumbs_up: Fetching all branches")

with console.status(" Fetching all merged branches...") as find_merges_status:
    cm_find_merges_command = "cm find merge \"where date >= '2023/10/15'\" --format=\"{srcbranch}\" --nototal"
    cm_find_merges_process = subprocess.run(cm_find_merges_command, capture_output=True, text=True)
    cm_find_merges_output = cm_find_merges_process.stdout
    merged_branches = cm_find_merges_output.replace('br:', '').split('\n')

    console.log(":thumbs_up: Fetching all merged branches")

with console.status(" Finding difference...") as analyze_status:
    not_integrated_branches = set(all_branches_with_owner.keys()).difference(set(merged_branches))
    not_integrated_branches_with_owner = {branch: details for branch, details in all_branches_with_owner.items() if
                                          branch in not_integrated_branches}

    console.log(":thumbs_up: Finding difference")


def pretty_date(date_str):
    date_obj = datetime.strptime(date_str, '%d/%m/%Y %H:%M:%S')
    return date_obj.strftime('%d %b, %Y')


table = Table()
table.add_column("Date")
table.add_column("Branch")
table.add_column("Owner")
for branch, details in not_integrated_branches_with_owner.items():
    table.add_row(pretty_date(details['date']), branch, details['owner'])

console.print()
console.print(table)
console.print()
