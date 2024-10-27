import datetime
import smbus
import time
import RPi.GPIO as GPIO
import pygame

pygame.mixer.init()
pygame.init()


GPIO.setmode(GPIO.BCM)
closePin = 23

bus = smbus.SMBus(1)
time.sleep(1)

prevTime = 0

arduinoAddress = 0x04

GPIO.setup(closePin, GPIO.IN, GPIO.PUD_UP)

def writeData(value):
    byteValue = StringToBytes(value)
    bus.write_i2c_block_data(arduinoAddress,0x00,byteValue)
    return -1

def StringToBytes(val):
    retVal = []
    for c in val:
        retVal.append(ord(c))
    return retVal


class Fader(object):
    instances = []
    def __init__(self, fname):
        super(Fader, self).__init__()
        #assert isinstance(fname, basestring)
        self.sound = pygame.mixer.Sound(fname)
        self.increment = 0.01 # tweak for speed of effect!!
        self.next_vol = 1 # fade to 100 on start
        Fader.instances.append(self)

    def fade_to(self, new_vol):
        # you could change the increment here based on something..
        self.next_vol = new_vol

    @classmethod
    def update(cls):
        for inst in cls.instances:
            curr_volume = inst.sound.get_volume()
            # print inst, curr_volume, inst.next_vol
            if inst.next_vol > curr_volume:
                inst.sound.set_volume(curr_volume + inst.increment)
            elif inst.next_vol < curr_volume:
                inst.sound.set_volume(curr_volume - inst.increment)

sound1 = Fader("piano_music.wav")
sound2 = Fader("synth.wav")
sound1.sound.play()
sound2.sound.play()
sound2.sound.set_volume(0)

# fading..
sound1.fade_to(0)
sound2.fade_to(1)


ledColor = 0
try:
    while True:
        currentSeconds = time.time()
        isSaving = False;
        if GPIO.input(closePin) == 0:
            time.sleep(.03)
            if GPIO.input(closePin) == 0:
                while GPIO.input(closePin) == 0:
                    time.sleep(0.02);
                raise KeyboardInterrupt
        Fader.update
        if currentSeconds - prevTime > 60.0 or isSaving:
            currentTime = datetime.datetime.now()
            timeString = str(currentTime.month)+" "+str(currentTime.day)+" "+str(currentTime.year)+" "+str(currentTime.hour).zfill(2)+" "+str(currentTime.minute).zfill(2)
            print(timeString)
            if isSaving:
                print("placeholder")
            print("Sending")
            writeData(timeString)
           # ledColor = bus.read_byte_data(arduinoAddress, 0)
            print(ledColor);
            prevTime = currentSeconds
            #time.sleep(60)
except KeyboardInterrupt:
    print("Ending program")
    #writeData("END!")
finally:
    GPIO.cleanup()
    pygame.quit()
