cmd_/home/shujie/test/Module.symvers := sed 's/ko$$/o/' /home/shujie/test/modules.order | scripts/mod/modpost -m -a   -o /home/shujie/test/Module.symvers -e -i Module.symvers   -T -
