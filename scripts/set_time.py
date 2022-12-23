import datetime
import sys
import time
import serial

now = datetime.datetime.now()
if len(sys.argv) == 1:
    print("Waiting for minute rollover...")
    remaining = 60 - now.second
    time.sleep(remaining)
    now += datetime.timedelta(seconds=remaining)

print("Setting time...")
s = serial.Serial("/dev/ttyACM0", 9600)
s.write(now.strftime("%y %m %d 0%w %H %M\n").encode('utf-8'))

msg = ""
while True:
    c = s.read(1)
    if c[0] == 0:
        break
    msg += c.decode('utf-8')
print(msg.strip("\n"))
s.close()
