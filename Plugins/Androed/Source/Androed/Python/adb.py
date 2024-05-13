import os
import subprocess
import time


def get_device_serial() -> [bool, str]:
    """
        Возвращает название первого девайса подключенного по ADB.
        Названием(SERIAL) может быть:
            - короткая хуйня из букв и цифр, если он был подключен через USB
            - IP адрес телефона, если он был подключен по WiFi;
            - длинная хуйня из букв и цифр, если он был подключен по WiFi, но автоматически
    """

    try:
        process = subprocess.run('adb devices'.split(), capture_output=True, text=True)
        output = process.stdout

        found_devices = output.strip().replace('\t', ' ').split("\n")[1:]
        online_devices = [device for device in found_devices if "device" in device]

        if online_devices:
            device = online_devices[0].replace('device', '').strip()
            return True, device
        else:
            return False, 'No online device found'

    except WindowsError:
        return False, 'ADB not installed'


def package_already_installed():
    success, device = get_device_serial()

    command = f'adb -s {device} shell pm list packages'
    process = subprocess.run(command.split(), capture_output=True, text=True)
    output = process.stdout

    if 'com.sistematik.somewhere' in output:
        return True
    return False


def uninstall_apk() -> [bool, str]:
    success, device = get_device_serial()

    command = f'adb -s {device} uninstall com.sistematik.somewhere'
    process = subprocess.run(command.split(), capture_output=True, text=True)

    if process.returncode == 0:
        return True, 'OK'
    else:
        return False, 'Package not found'


def upload_apk() -> [bool, str]:
    success, device = get_device_serial()
    apk = os.path.normpath(
        os.path.join(os.path.dirname(__file__), r'../../../Binaries/Android/SOMEWHERE_universal.apk')
    )

    command = ['adb', '-s', device, 'push', apk, '/data/local/tmp/']
    process = subprocess.run(command, capture_output=True, text=True)

    if process.returncode == 0:
        return True, 'OK'
    else:
        return False, 'Cannot push apk file to device'


def install_apk() -> [bool, str]:
    success, device = get_device_serial()
    apk = os.path.normpath(
        os.path.join(os.path.dirname(__file__), r'../../../../../Binaries/Android/SOMEWHERE_universal.apk')
    )

    # command = ['adb', '-s',  device, 'shell', 'pm', 'install', '-r', '-d', '/data/local/tmp/SOMEWHERE_universal.apk']
    command = ['adb', '-s',  device, 'install', '-r', '-d', apk]
    process = subprocess.run(command, capture_output=True, text=True)

    if process.returncode == 0:
        return True, 'OK'
    else:
        return False, 'Cannot install package'
