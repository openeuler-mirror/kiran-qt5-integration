#!/bin/bash
#该脚本主要负责:
# 1. 调用python脚本根据colors描述文件提取相应的颜色信息并进行相应的颜色转换(ColorEffect)到_global.scss
# 2. 通过sassc将该项目中内置的scss与_global.scss,_functions.scss生成对应的颜色表一起转换至css之中

set -e

# Usage: render_theme <colorscheme> <base-colorscheme> <generate-dir> <generate-filename>
# example: render_theme dark.colors base.colors /tmp/dark/ dark
render_theme () {
  # step-1: 提取colorscheme与base-colorscheme颜色表中所定义的数据提取到generate-dir下的_global.scss之中
  python3 extract_color_from_scheme.py -c "$1" -b "$2" -d "$3"
  # step-2: 通过sassc将项目内置的scss与_global.scss,functions.scss转换为一个新的css文件
  sassc -I "$3" ./data/qt5.scss "$3/$4-colors.css"
}

COLOR_SCHEME_FILE=""
BASE_SCHEME_FILE=""
THEME_NAME=""
THEME_BUILD_DIR=""

while [ "$#" -gt 0 ]; do
    case "$1" in
        -h|--help)
            echo "help"
            echo "$0: build kiran qt theme"
            echo "Usage: $0 .."
            exit 0
        ;;
        -c|--color-scheme-file)
            shift
            COLOR_SCHEME_FILE="$1"
        ;;
        -b|--base-scheme-file)
            shift
            BASE_SCHEME_FILE="$1"
        ;;
        -t|--theme-name)
            shift
            THEME_NAME="$1"
        ;;
        -d|--dest-dir)
            shift
            THEME_BUILD_DIR="$1"
        ;;
    esac
    shift
done

render_theme "${COLOR_SCHEME_FILE}" "${BASE_SCHEME_FILE}"  "${THEME_BUILD_DIR}" "${THEME_NAME}"

