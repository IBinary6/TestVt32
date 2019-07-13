
#2019-7-13说明
增加了VMXON 以及 VMXOFF的汇编方法.
定义了VMXON需要用到的结构体
修改了检测VT是否可以支持.主要是增加了对CR0.PG CR0.PE位的支持.
添加了 EFLAGS CR0 - CR4 结构体.主要是位域.
