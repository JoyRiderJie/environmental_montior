# Environmental_Montior

## 介绍
> &emsp;&emsp;本项目是一个基于FreeRTOS的STM32实战项目，项目采用一主多从的环境监测系统（温度、湿度、光照、烟雾）。主节点是一个中继节点，它既需要向下**收集**各个分节点通过LORA发送的数据，再处理后经WIFI发送到服务器；也需要经WIFI接收服务器发送过来的数据，经处理后通过LORA发送到各个分节点。

## 项目环境说明

> * 开发板：选择**STM32ZET6作为系统主板**，采用**STM32C8T6作为系统节点**；
> * 操作系统：系统主板与节点均**采用FreeRTOS作为操作系统**；
> * IED：采用Keil5作为集成开发环境；
> * 数据传输：本系统采用**LORA作为主板与节点之间的传输媒介，采用WIFI作为主板与服务器的数据收发方式**；
> * 数据传输格式：为了系统与主板更好解析数据，**采用jsion格式传输数据**；


## git commit提交规范

git commit提交描述信息格式: `<type>(<scope>): <subject>`，例如：add(sensor/src):添加一个烟雾传感器测量烟雾值。

**type: 操作类型**

|   类型   |       描述       |
| :------: | :--------------: |
|   add    |     新增功能     |
|  update  |     更新功能     |
|  delete  |     删除功能     |
|   fix    |     修复 BUG     |
| refactor |     代码重构     |
|  revert  |  撤销上次commit  |
|  style   |   代码风格更改   |
|   docs   | 修改文档相关内容 |

**scope: 影响范围**

用于说明本次 commit 的影响范围，比如: 具体功能或模块，控制器层，业务层，模型层等，视项目不同而不同。

 **subject: 简单描述**

本次 commit 的简单描述，一般不超过 50 个字符。推荐以动词开头: 新增，修改，设置撤销等等。

#### 项目目录说明

> |------App：STM32外设目录
>
> |&emsp;&emsp;&emsp;|---inc：STM32外设实现的.h头文件目录
>
> |&emsp;&emsp;&emsp;|---src：STM32外设实现的.c文件目录
>
> |------FreeRTOS_Source：FreeRTOS操作系统目录
>
> |&emsp;&emsp;&emsp;|---inc：FreeRTOS实现的.h头文件目录
>
> |&emsp;&emsp;&emsp;|---portable：可替换的FreeRTOS内核管理目录
>
> |&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;|---MemMang：FreeRTOS内存管理文件夹
>
> |&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;|---RVDS：IDE为keil的Cortex必要文件
>
> |------Sensor：传感器目录
>
> |&emsp;&emsp;&emsp;|---inc：传感器实现的.h头文件目录
>
> |&emsp;&emsp;&emsp;|---src：传感器实现的.c文件目录
>
> |------STM32Project：STM32项目工程的目录
