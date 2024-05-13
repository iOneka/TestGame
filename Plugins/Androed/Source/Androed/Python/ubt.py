import json
import os
import subprocess
import time
import winreg
import utils


def fix_engine(engine):
    gradle = engine + r'\Build\Android\Java\gradle\app\build.gradle'

    with open(gradle, 'r') as file:
        lines = file.readlines()

    dependency = "implementation (\"androidx.browser:browser:1.5.0\"){ force = true }"
    dependency_block = False
    dependency_found = False

    for index, line in enumerate(lines):
        stripped_line = line.strip()

        if stripped_line == 'dependencies {':
            dependency_block = True
        elif stripped_line == '}' and dependency_block:
            dependency_block = False
            if not dependency_found:  # Если зависимость не найдена, добавляем ее перед закрывающей скобкой
                lines.insert(index, '	' + dependency + '\n')
        elif dependency_block and dependency in stripped_line:
            dependency_found = True

    if dependency_found:
        pass
    else:
        with open(gradle, 'w') as file:
            file.writelines(lines)


def get_apk_path():
    """
        Возвращает путь apk файла
    """
    apk_path = os.path.join(os.path.dirname(__file__), r'../../../Binaries/Android/SOMEWHERE_universal.apk')
    apk_path = os.path.normpath(apk_path)

    if os.path.exists(apk_path):
        return apk_path
    return None


def get_build_command(engine, project):
    command = fr'{engine}\Build\BatchFiles\RunUAT.bat BuildCookRun ' \
              fr'-nop4 -utf8output -nocompileeditor -skipbuildeditor -cook ' \
              fr'-project="{project}" -target=SOMEWHERE ' \
              fr'-unrealexe="{engine}\Binaries\Win64\UnrealEditor-Cmd.exe" ' \
              fr'-map=+L_IslandEsterhull+L_IslandEsterhull2 ' \
              fr'-CookCultures=en+ru-RU  ' \
              fr'-platform=Android -cookflavor=Multi ' \
              fr'-installed -stage -package -build -pak -iostore -compressed -prereqs ' \
              fr'-clientconfig=Development -nodebuginfo -nocompile -nocompileuat -distribution'

    return command


def build_android(project, engine) -> [bool, str]:
    command = get_build_command(engine, project)
    log = ""

    fix_engine(engine)

    try:
        # Запускаем команду
        build_process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # Выводим вывод
        while True:
            build_output = build_process.stdout.readline().decode("utf-8").strip()
            if build_process.poll() is not None:
                break

            if build_output:
                log += build_output + '\n'
                # print(build_output)

                if 'BUILD COMMAND STARTED' in build_output:
                    yield True, 'BUILD COMMAND STARTED'
                elif 'BUILD COMMAND COMPLETED' in build_output:
                    yield True, 'BUILD COMMAND COMPLETED'

                elif 'COOK COMMAND STARTED' in build_output:
                    yield True, 'COOK COMMAND STARTED'
                elif 'COOK COMMAND COMPLETED' in build_output:
                    yield True, 'COOK COMMAND COMPLETED'

                elif 'STAGE COMMAND STARTED' in build_output:
                    yield True, 'STAGE COMMAND STARTED'
                elif 'STAGE COMMAND COMPLETED' in build_output:
                    yield True, 'STAGE COMMAND COMPLETED'

                elif 'PACKAGE COMMAND STARTED' in build_output:
                    yield True, 'PACKAGE COMMAND STARTED'
                elif 'PACKAGE COMMAND COMPLETED' in build_output:
                    yield True, 'PACKAGE COMMAND COMPLETED'

        # Что-то пошло не так
        if build_process.returncode != 0:
            yield False, log

    # Точно что-то пошло не так
    except WindowsError:
        yield False, log
