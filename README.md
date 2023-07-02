## TinyLex&Yacc

* 本项目为本科编译原理实践课程作业

* 部分Yacc的架构参考了项目TinyYacc（[lishuhuakai/TinyYACC: 用cpp编写的一个实现了最核心功能的YACC,供练习使用. (github.com)](https://github.com/lishuhuakai/TinyYACC)）

* 由于c++文法为本人参考[C 词法语法 | Microsoft Learn](https://learn.microsoft.com/zh-cn/cpp/c-language/lexical-grammar?view=msvc-170) 手写的，尚存在一些问题，并非LR(1)文法，部分语句存在冲突，所以得到的语法解析树在遇到某些特定的情况时会出错（比如已经发现的有 `while (!bool)`中`!`不会识别到`expression`中；`int main()`中，`()`中不能为空，需要有参数）

* 目前只实现了构造语法解析树，并未进一步生成中间代码
