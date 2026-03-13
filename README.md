# newFirmware

Quick reference for backing up the EMO ESP32 flash, extracting the SPIFFS
partition, and flashing custom firmware.

These commands assume:

- You are using PowerShell on Windows.
- You replace `COMx` with the actual ESP32 serial port.
- You run commands from the project root when using `.\build\...` paths.

## Flash Layout Used By This Project

From `partitions.csv`:

- Full flash size: `0x1000000` (`16 MB`)
- App slot `ota_0`: `0x20000`
- SPIFFS partition `storage`: `0xA20000`
- SPIFFS size: `0x100000`

SPIFFS settings used by this project:

- Block size: `4096`
- Page size: `256`

## Full Backup

Back up the full 16 MB flash. It is a good idea to dump it at least 5 times and
compare hashes so you know the backup is stable.

```powershell
esptool.py --chip esp32 -p COM25 -b 115200 read_flash 0x0 0x1000000 "$env:USERPROFILE\Desktop\esp32_full_16MB.bin"
```

## Full Restore

This rewrites the entire 16 MB flash from a full-chip backup image.

```powershell
esptool.py --chip esp32 -p COM11 -b 115200 write_flash --flash_mode keep --flash_freq keep --flash_size keep 0x0 "$env:USERPROFILE\Desktop\esp32_full_16MB.bin"
```

## Backup SPIFFS Only

This reads only the `storage` SPIFFS partition instead of the whole flash.

```powershell
esptool.py --chip esp32 -p COMx -b 115200 read_flash 0xA20000 0x100000 "$env:USERPROFILE\Desktop\storage.bin"
```

## Extract Files From SPIFFS

`spiffsgen.py` only creates SPIFFS images. It does not unpack them.
To extract files from `storage.bin`, use `mkspiffs` with the matching geometry:

```powershell
mkspiffs -u .\spiffs_extracted -b 4096 -p 256 -s 0x100000 "$env:USERPROFILE\Desktop\storage.bin"
```

Notes:

- The output folder will be created as `.\spiffs_extracted`.
- For this firmware image, the extracted files are motion files under `mot\`.
- If you want to dump the partition directly from the device and then unpack it,
  run the SPIFFS backup command first, then the `mkspiffs` command above.

## Flash SPIFFS Back To The Device

If you already have a SPIFFS image to write back, flash it at the `storage`
partition offset:

```powershell
esptool.py --chip esp32 -p COMx -b 115200 write_flash --flash_mode keep --flash_freq keep --flash_size keep 0xA20000 .\build\storage_idf55.bin
```

## Flash Custom Firmware

If you only want to replace the application in `ota_0`, flash the app binary at
`0x20000`:

```powershell
esptool.py --chip esp32 -p COMx -b 115200 write_flash --flash_mode dio --flash_freq 40m --flash_size 16MB 0x20000 .\build\emo_firmware.bin
```

After `base-firmware.bin` is flashed, connect to the ESP32 hotspot and set a
Wi-Fi network. Once that is done, you can access the web page via
`node-manager.local`.

## Install Apps From The UI

To flash apps, open the Apps Manager in the web UI and select the app you want
to install.

If the app you are installing is a test app, remember to remove the auto-start
check before installing it.

## Safety Notes

- Double-check the COM port before writing.
- A full restore overwrites everything, including both app slots and SPIFFS.
- An app-only flash at `0x20000` does not restore SPIFFS contents.
- Keep at least one untouched full 16 MB backup before testing custom builds.
