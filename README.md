本库是根据斯坦福大学 CS106B 系列课程，以及本人的学习经验所编写的内容。
为了提高学习效率，我们直接从 Assignments 出发，带着问题去课程录像以及课件中找答案。
由于课程存在部分 Assignment 不开放的情况，我们只好根据不同的开放状况来选择作业内容。到目前为止，存在以下对应关系：

- Assignment1：Spring 2024 Assignment1
- Assignment2：Summer 2022 Assignment2
- Assignment3：无回溯内容的 Summer 2022 Assignment3
- Assignment4：Summer 2022 Assignment3 的回溯部分 
- Assignment5：Winter 2022 的 Assignment6 
- Assignment6：Summer 2022 的 Assignment5
- Assignment7：Summer 2022 的 Assignment6
- Assignment8：Winter 2022 的 Assignment7
## Qt 环境配置
大体按照学院课程官网的教程配置，这里主要提供两个配置时问题的解决方法：

1. 无法加载程序输入点 xxxxxx 于动态链接库 Qt6Core/Qt6Widgets/Qt6Gui.dll
2. this application failed to start because no qt platform plugin could be initialized
### 问题 1
将 `Qt版本号/mingw_64/bin` 路径添加到系统变量。
或者将缺少的动态库，以及 libstdc++-6.dll 添加到构建后得到可执行文件的所在路径下。
### 问题 2
给用户系统变量加上 Qt 的 plugins/platforms 所在路径的记录：
`变量名：QT_QPA_PLATFORM_PLUGIN_PATH`
`变量值：xxxx\plugins\platforms`
## 参考文章

- [https://forum.qt.io/topic/135430/the-procedure-entry-point-_znst3pmr20get_default_resourceev-could-not-be-located-in-the-dynamic-library-qt6gui-dll-qt6widgets-and-qt6core-dll/11](https://forum.qt.io/topic/135430/the-procedure-entry-point-_znst3pmr20get_default_resourceev-could-not-be-located-in-the-dynamic-library-qt6gui-dll-qt6widgets-and-qt6core-dll/11)
- [https://www.cnblogs.com/lili414/p/16906310.html](https://www.cnblogs.com/lili414/p/16906310.html)
