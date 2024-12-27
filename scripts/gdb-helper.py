import gdb

# basic settings
gdb.execute('set pagination off')
gdb.execute('set confirm off')

# set breakdpoint
gdb.execute('set env TSAN_OPTIONS=halt_on_error=1')
gdb.execute('b main')
gdb.execute('r')
gdb.execute('b exit')
gdb.execute('b _exit')
gdb.execute('catch syscall exit_group')
gdb.execute('c')

