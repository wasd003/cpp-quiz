假设原本thread-B的cv等待条件是X，那么现在cv等待条件是X || exit_flag。其中exit_flag是thread-A和thread—B的共享变量。
在thread-B中主动检查exit_flag，如果exit_flag是true则跳出死循环。
