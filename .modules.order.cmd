cmd_/home/shujie/test/modules.order := {   echo /home/shujie/test/hello_world.ko; :; } | awk '!x[$$0]++' - > /home/shujie/test/modules.order
