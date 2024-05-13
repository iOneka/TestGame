import argparse
import subprocess
import sys
import time
import re

import utils
from utils import Spinner
from utils import ConsoleColors

import adb
import ubt

parser = argparse.ArgumentParser()
parser.add_argument('--project')
parser.add_argument('--engine')
args, unknown = parser.parse_known_args()

print('')
print(ConsoleColors.WARNING + '██████╗ ██████╗ ███████╗██████╗  █████╗ ██████╗ ███████╗' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██████╔╝██████╔╝█████╗  ██████╔╝███████║██████╔╝█████╗' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██╔═══╝ ██╔══██╗██╔══╝  ██╔═══╝ ██╔══██║██╔══██╗██╔══╝' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██║     ██║  ██║███████╗██║     ██║  ██║██║  ██║███████╗' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '╚═╝     ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝' + ConsoleColors.RESET)
print('')

PREPARATION_FAILED = False

print(ConsoleColors.WARNING + "╔═█ PROJECT\t" + ConsoleColors.RESET, end='')
Success, Result = True, args.project
if Success:
    print(Result)
else:
    print(ConsoleColors.FAIL + Result + ConsoleColors.RESET)
    PREPARATION_FAILED = True

print(ConsoleColors.WARNING + "╠═█ ENGINE\t" + ConsoleColors.RESET, end='')
Success, Result = True, args.engine
if Success:
    print(Result)
else:
    print(ConsoleColors.FAIL + Result + ConsoleColors.RESET)
    PREPARATION_FAILED = True

print(ConsoleColors.WARNING + "╚═█ DEVICE\t" + ConsoleColors.RESET, end='')
Success, Result = adb.get_device_serial()
if Success:
    print(Result)
else:
    print(ConsoleColors.FAIL + Result + ConsoleColors.RESET)
    PREPARATION_FAILED = True

if PREPARATION_FAILED:
    print()
    input(ConsoleColors.FAIL + 'PREPARATION FAILED ' + ConsoleColors.RESET + 'Press Enter to exit...')
    sys.exit(1)
else:
    print()
    print(ConsoleColors.OKGREEN + 'OK' + ConsoleColors.RESET)

print('')
print(ConsoleColors.WARNING + '██████╗ ██╗   ██╗██╗██╗     ██████╗' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██╔══██╗██║   ██║██║██║     ██╔══██╗' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██████╔╝██║   ██║██║██║     ██║  ██║' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██╔══██╗██║   ██║██║██║     ██║  ██║' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██████╔╝╚██████╔╝██║███████╗██████╔╝' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '╚═════╝  ╚═════╝ ╚═╝╚══════╝╚═════╝' + ConsoleColors.RESET)
print('')

BUILD_FAILED = False
BUILD_LOG = ""

start_time = time.time()

with Spinner():
    for buildstep in ubt.build_android(args.project, args.engine):
        if buildstep[0]:

            if buildstep[1] == 'BUILD COMMAND STARTED':
                print('\b' + ConsoleColors.WARNING + "╔═█ BUILD\t" + ConsoleColors.RESET, end='')
            elif buildstep[1] == 'BUILD COMMAND COMPLETED':
                print(f'\b{utils.seconds_to_string(time.time() - start_time)}')
                start_time = time.time()

            elif buildstep[1] == 'COOK COMMAND STARTED':
                print('\b' + ConsoleColors.WARNING + "╠═█ COOK\t" + ConsoleColors.RESET, end='')
            elif buildstep[1] == 'COOK COMMAND COMPLETED':
                print(f'\b{utils.seconds_to_string(time.time() - start_time)}')
                start_time = time.time()

            elif buildstep[1] == 'STAGE COMMAND STARTED':
                print('\b' + ConsoleColors.WARNING + "╠═█ STAGE\t" + ConsoleColors.RESET, end='')
            elif buildstep[1] == 'STAGE COMMAND COMPLETED':
                print(f'\b{utils.seconds_to_string(time.time() - start_time)}')
                start_time = time.time()

            elif buildstep[1] == 'PACKAGE COMMAND STARTED':
                print('\b' + ConsoleColors.WARNING + "╚═█ PACKAGE\t" + ConsoleColors.RESET, end='')
            elif buildstep[1] == 'PACKAGE COMMAND COMPLETED':
                print(f'\b{utils.seconds_to_string(time.time() - start_time)}')
                start_time = time.time()

        else:
            BUILD_FAILED = True
            BUILD_LOG = buildstep[1]

    if BUILD_FAILED:
        print('\b')
        input(ConsoleColors.FAIL + 'BUILD FAILED ' + ConsoleColors.RESET + 'Press Enter to show log...')
        print()
        print(BUILD_LOG)
        print()
        input('Press Enter to exit')
        sys.exit(1)
    else:
        print('\b\b')
        print(ConsoleColors.OKGREEN + 'OK' + ConsoleColors.RESET)

print()
print(ConsoleColors.WARNING + '██████╗ ███████╗██████╗ ██╗      ██████╗ ██╗   ██╗' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██╔══██╗██╔════╝██╔══██╗██║     ██╔═══██╗╚██╗ ██╔╝' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██║  ██║█████╗  ██████╔╝██║     ██║   ██║ ╚████╔╝' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██║  ██║██╔══╝  ██╔═══╝ ██║     ██║   ██║  ╚██╔╝' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '██████╔╝███████╗██║     ███████╗╚██████╔╝   ██║' + ConsoleColors.RESET)
print(ConsoleColors.WARNING + '╚═════╝ ╚══════╝╚═╝     ╚══════╝ ╚═════╝    ╚═╝' + ConsoleColors.RESET)
print()

print(ConsoleColors.WARNING + "╔═█ INSTALL\t" + ConsoleColors.RESET, end='')
with Spinner():
    Success, Result = adb.install_apk()
if Success:
    print(ConsoleColors.OKGREEN + Result + ConsoleColors.RESET)
else:
    print(ConsoleColors.FAIL + Result + ConsoleColors.RESET)
    print()
    input(ConsoleColors.FAIL + 'INSTALL FAILED ' + ConsoleColors.RESET + 'Press Enter to exit...')
    sys.exit(1)


# Открываем
adb_run_command = f"adb -s {adb.get_device_serial()[1]} shell monkey -p com.sistematik.somewhere 1"
adb_run_process = subprocess.run(adb_run_command.split(), capture_output=True, text=True)
if adb_run_process.returncode != 0:
    print(ConsoleColors.FAIL + 'Cannot open game' + ConsoleColors.RESET)
    print()
    input(ConsoleColors.FAIL + 'FAIL ' + ConsoleColors.RESET + 'Press Enter to exit...')
    sys.exit(1)

# Unreal логи
adb_logtail_command = f"adb -s {adb.get_device_serial()[1]} shell " \
                      f"tail -F -n +1 " \
                      f"/sdcard/Android/data/com.sistematik.somewhere/files/UnrealGame/SOMEWHERE/SOMEWHERE/Saved/Logs/SOMEWHERE.log"

try:
    # Запускаем команду
    adb_logtail_process = subprocess.Popen(adb_logtail_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Выводим вывод
    while True:
        adb_logtail_output = adb_logtail_process.stdout.readline().decode("utf-8")
        if adb_logtail_process.poll() is not None:
            break
        if adb_logtail_output:
            adb_logtail_output = adb_logtail_output.strip()

            if re.search(r'Blueprint', adb_logtail_output, re.IGNORECASE):
                print(ConsoleColors.OKBLUE + adb_logtail_output + ConsoleColors.RESET)

            elif re.search(r'Error', adb_logtail_output, re.IGNORECASE):
                print(ConsoleColors.FAIL + adb_logtail_output + ConsoleColors.RESET)

            elif re.search(r'Fail', adb_logtail_output, re.IGNORECASE):
                print(ConsoleColors.FAIL + adb_logtail_output + ConsoleColors.RESET)

            else:
                print(adb_logtail_output.strip())

# Точно что-то пошло не так
except WindowsError:
    print(ConsoleColors.OKGREEN + 'PIZDEC ' + ConsoleColors.RESET)

input(ConsoleColors.OKGREEN + 'OK ' + ConsoleColors.RESET + 'Press Enter to exit')
