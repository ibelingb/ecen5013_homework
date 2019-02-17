set auto-load safe-path /
set sysroot /home/marko/installs/buildroot/output/staging
target extended-remote 192.168.1.220:10000
remote put /home/marko/repos/ecen5013_hw/hw3/problem2/file_io_debug 192.168.1.220:10000
set remote exec-file file_io_debug
