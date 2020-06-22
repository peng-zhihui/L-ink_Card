# [L-ink]电子墨水屏NFC智能卡片
> 这是为了解决个人使用IC卡时遇到的一些痛点设计的一个迷你NFC智能卡片，基于STM32L051和ST25DV。
>
> 想着既然要自制这玩意那就往炫酷了做，于是很自觉地把电子墨水屏也加上了，顺手也写了个配套APP用于显示内容定制，总之是个比较有意思的小项目。方案不算复杂，第一版设计也存在一些值得改进的地方，后续有空会继续优化方案设计的。

![](Image\L-ink V0.5.jpg)

## 演示视频

Bilibili链接：https://www.bilibili.com/video/BV1Cf4y1y7KT/

## 项目文件说明

* **Hardware**：source里面是电路原理图和PCB文件，使用Altium Designer打开；release里面是gerber文件可以直接发给厂家打样，也包含了元器件BOM表。
* **Firmware**：STM32L051的固件，我提供了Clion和Keil两个版本的工程文件，源码内容是一致的，只是个人对Clion爱得深沉。
* **3D Model**：屏幕架和外壳的3D文件，step格式一般3D建模软件都能打开编辑，STL格式用于直接3D打印。
* **Android**：配套的APP安装包。

## 固件编译说明

STM32因为使用了L0系列的MCU，所以如果你是第一次在你的电脑上开发STM32L0，那么需要先装一下L0的固件支持包，有两种方式：

1. 使用STM32CubMX安装：

   ![](Image/bsp1.jpg)

   ![](Image/bsp2.jpg)

2. 使用Keil的Package Installer安装：

   ![](Image/bsp3.jpg)

   ![](Image/bsp4.jpg)

然后就不会报头文件的错误了。

有几点注意的：

* 如果想在Clion中编译工程的话可以参考我这篇教程：https://zhuanlan.zhihu.com/p/145801160
* Clion中OpenOCD默认是使用ST-Link下载，其他下载器自己修改工程目录下的stlink.cfg文件

## 关于配套APP

Android APP目前只是实现了功能，这是基于我以前做的一个算法验证框架改的，UI还没有重新做设计，可以等后续更新。

## 教程整理中...
