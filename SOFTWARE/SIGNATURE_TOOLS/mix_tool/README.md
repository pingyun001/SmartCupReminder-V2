# 功能介绍
这是一个基于python的hex合并工具，用于将bootloader和app合并成一个完整的hex文件，以便工厂批量烧录
# 如何使用

## 环境搭建
本项目需使用python环境，需确保安装，同时需要安装IntelHex库，使用如下命令安装：
```
pip install intelhex
```

## 如何使用
将待合并的bootloader和app分别编译成hex文件，并放到./input目录下，然后双击批处理脚本运行，即可生成合并后的hex文件，并输出到./output目录下

# 版权
bilibili 平韵の小窝 原创作品

仅限个人学习使用，严禁无授权的商业用途！
