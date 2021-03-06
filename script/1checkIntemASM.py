import os
import sys
import platform

def which(program):

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

for exe in os.listdir(sys.argv[1]):
    exe = sys.argv[1] + exe
    asm = 'addpd\|addsd\|paddq'
    if platform.system() == 'Darwin':
        cmd_line = "/opt/local/x86_64-apple-darwin14.0.0/bin/objdump -M intel -d %s | grep -w '%s' > objdump.out" % (exe,asm)
    else:
        cmd_line = "objdump -M intel -d %s | grep -w '%s' > objdump.out" % (exe,asm)
    if os.path.isfile(exe) and os.access(exe, os.X_OK):
        #print cmd_line
        os.system(cmd_line)
        if len(open('objdump.out','r').readlines()) > 1:
			print exe, ' contains intel SSE/MMX'
