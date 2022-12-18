# myjson
*** 自己手写的tinyJSON解析库
- 用了C++17 variant取代enum增强类型安全
- 用shared_ptr控制内存，保证内存安全
- 运用右值引用和移动拷贝避免字符串的重复拷贝
- 运用unordered_map存放object类型
