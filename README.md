# L-ink_Card
Smart NFC &amp; ink-Display Card

视频链接：https://www.bilibili.com/video/BV1Cf4y1y7KT/

STM32固件不是keil工程（当然也可以自己把源文件复制到keil工程下，代码不多），有几点注意的：
* 工程依赖ST官方的NFC库，可以在STM32CubeMX中下载
* 如果想在Clion中编译工程的话可以参考我这篇教程：https://zhuanlan.zhihu.com/p/145801160
* 如果想在Keil中编译工程的话可以打开项目的.ioc文件，生成MDK工程即可（NFC库也可以在这里下载）

Android APP目前只是实现了功能，基于我以前做的一个算法验证框架做的，UI还没有改动，等后续更新；目前代码还有很多dirty code，仅供有基础的同学参考一下，后面会统一清理封装好的。



## 其余教程整理中...
