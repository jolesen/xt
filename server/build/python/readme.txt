xls:
数据类型T: bool, uint, string, S2UInt, S3UInt
格式：
    普通模式: 字段名(T[, const]), []内的为可选，const标识需要导出const常量
    数组模式: 字段名(list|T)


TODO:
1. 增加脚本判断协议是否被使用，可以删除不必要的协议
2. 每个协议或结构体增加clear函数，这样可以清除数据，便于对象重用。
