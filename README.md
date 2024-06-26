# Firmware of JianWei Power-supply Board

## versions
- PCB-v2 uses SoC stm32f051x8
- PCB-v3 uses SoC stm32f103xb

## initialization

```shell
west init -m <git_url> --mr v3-dev <your_workspace>
cd <your_workspace>`
west update
```

## build
For PCB-v2:
`west build -p -b jw_pwb/stm32f051x8 -d build pwb_firmware.git/app`

For PCB-v3:
`west build -p -b jw_pwb/stm32f103xb -d build pwb_firmware.git/app`

## flashing (linux)

### use native bootloader and uart1
`sudo stm32flash -w <path/to/zephyr.hex> -g 0x0 -v </path/to/tty/device>`

### use SWD port and J-Link debug probe
`west flash -d build -r jlink`

## check memory footprint

### RAM usage
`west build -d <build-directory> -t ram_report`

### ROM usage
`west build -d <build-directory> -t rom_report`
