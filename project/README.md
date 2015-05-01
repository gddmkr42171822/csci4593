#4593Project

You can test the program with a single trace line using the command: 
```sh
echo '<instruction> <address> 0' | ./memsim <config_file>
```
or run with a trace file
```sh
cat '<trace_file>' | ./memsim <config_file>
```

#TESTING
##Small trace tr1 
Final cache contents are correct for:
- [x] Default
- [x] L1-2way
- [x] All-2way
- [x] L2-4way
- [x] All-4way
- [x] L1-8way
- [x] L2-Big
- [x] L1-Small
- [x] L1-Small-4way
- [x] All-FA
- [x] All-FA-L2big
##Small trace tr2
Final cache contents are correct for:
- [x] Default
##Small trace tr3
Final cache contents are correct for:
- [x] Default
##Small trace tr4
Final cache contents are correct for:
- [x] Default
##Small trace tr5
Final cache contents are correct for:
- [x] Default
##Small trace tr6
Final cache contents are correct for:
- [] Default