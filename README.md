# Firmware of JianWei Power-supply Board

## initialization

```shell
west init -m <git_url> <your_workspace>
cd <your_workspace>`
west update
```

## build
`west build -p -b jw_pwb -d build pwb_firmware.git/app`

## flashing (linux)

`sudo stm32flash -w <path/to/zephyr.hex> -g 0x0 -v </path/to/tty/device>`

## check memory footprint

### RAM usage
`west build -d <build-directory> -t ram_report`

### ROM usage
`west build -d <build-directory> -t rom_report`
